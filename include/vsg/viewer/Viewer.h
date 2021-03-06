#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/viewer/View.h>
#include <vsg/viewer/Window.h>

#include <vsg/ui/ApplicationEvent.h>

#include <map>

namespace vsg
{

    class VSG_DECLSPEC Viewer : public Inherit<Object, Viewer>
    {
    public:
        Viewer();

        using Windows = std::vector<ref_ptr<Window>>;
        using Views = std::vector<ref_ptr<View>>;

        struct PerDeviceObjects
        {
            Windows windows;
            VkQueue graphicsQueue;
            VkQueue presentQueue;
            ref_ptr<Semaphore> renderFinishedSemaphore;

            // cache data to used each frame
            std::vector<uint32_t> imageIndices;
            std::vector<VkSemaphore> signalSemaphores;
            std::vector<VkCommandBuffer> commandBuffers;
            std::vector<VkSwapchainKHR> swapchains;
            std::vector<VkPipelineStageFlags> waitStages;
        };

        using DeviceMap = std::map<ref_ptr<Device>, PerDeviceObjects>;

        /// add Window to Viewer
        void addWindow(ref_ptr<Window> window);

        Windows& windows() { return _windows; }
        const Windows& windows() const { return _windows; }

        /// add View to Viewer
        void addView(ref_ptr<View> view);

        Views& getViews() { return _views; }
        const Views& getViews() const { return _views; }

        clock::time_point& start_point() { return _start_point; }
        const clock::time_point& start_point() const { return _start_point; }

        FrameStamp* getFrameStamp() { return _frameStamp; }
        const FrameStamp* getFrameStamp() const { return _frameStamp; }

        /// return true if viewer is valid and active
        bool active() const;

        /// schedule closure of the viewer and associated windows, after a call to Viewer::close() the Viewer::active() method will return false
        void close() { _close = true; }

        /// poll the events for all attached widnows, return true if new events are available
        bool pollEvents(bool discardPreviousEvents = true);

        /// get the current set of Events that are filled in by prior calls to pollEvents
        Events& getEvents() { return _events; }

        /// get the const current set of Events that are filled in by prior calls to pollEvents
        const Events& getEvents() const { return _events; }

        /// add event handler
        void addEventHandler(ref_ptr<Visitor> eventHandler) { _eventHandlers.emplace_back(eventHandler); }

        void addEventHandlers(EventHandlers&& eventHandlers) { _eventHandlers.splice(_eventHandlers.end(), eventHandlers); }

        /// get the const list of EventHandlers
        EventHandlers& getEventHandlers() { return _eventHandlers; }

        /// get the const list of EventHandlers
        const EventHandlers& getEventHandlers() const { return _eventHandlers; }

        /// poll for pending events and place them in the Events list and update generate FrameStamp to signify the advancement to a new frame.
        void advance();

        /// pass the Events into the any register EventHandlers
        void handleEvents();

        void reassignFrameCache();

        bool aquireNextFrame();

        bool populateNextFrame();

        bool submitNextFrame();

    protected:
        virtual ~Viewer();

        bool _close = false;

        ref_ptr<FrameStamp> _frameStamp;

        Windows _windows;
        Views _views;

        DeviceMap _deviceMap;

        clock::time_point _start_point;
        Events _events;
        EventHandlers _eventHandlers;
    };

} // namespace vsg
