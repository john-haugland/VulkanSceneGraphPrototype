#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <fstream>
#include <vsg/vk/Device.h>

namespace vsg
{
    template<typename T>
    bool readFile(T& buffer, const std::string& filename)
    {
        std::ifstream fin(filename, std::ios::ate | std::ios::binary);
        if (!fin.is_open()) return false;

        size_t fileSize = fin.tellg();

        using value_type = typename T::value_type;
        size_t valueSize = sizeof(value_type);
        size_t bufferSize = (fileSize + valueSize - 1) / valueSize;
        buffer.resize(bufferSize);

        fin.seekg(0);
        fin.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        fin.close();

        // buffer.size() * valueSize

        return true;
    }

    class VSG_DECLSPEC Shader : public Inherit<Object, Shader>
    {
    public:
        using Source = std::string;
        using SPIRV = std::vector<uint32_t>;

        Shader(VkShaderStageFlagBits stage, const std::string& entryPointName, const SPIRV& spirv);
        Shader(VkShaderStageFlagBits stage, const std::string& entryPointName, const Source& source);
        Shader(VkShaderStageFlagBits stage, const std::string& entryPointName, const Source& source, const SPIRV& spirv);

        VkShaderStageFlagBits& stage() { return _stage; }
        VkShaderStageFlagBits stage() const { return _stage; }

        std::string& entryPointName() { return _entryPointName; }
        const std::string& entryPointName() const { return _entryPointName; }

        std::string& source() { return _source; }
        const std::string& source() const { return _source; }

        SPIRV& spirv() { return _spirv; }
        const SPIRV& spirv() const { return _spirv; }

        using Result = vsg::Result<Shader, VkResult, VK_SUCCESS>;
        static Result read(VkShaderStageFlagBits stage, const std::string& entryPointName, const std::string& filename);

        void read(Input& input) override;
        void write(Output& output) const override;

    protected:
        virtual ~Shader();

        VkShaderStageFlagBits _stage;
        std::string _entryPointName;
        std::string _source;
        SPIRV _spirv;
    };
    VSG_type_name(vsg::Shader);

    class VSG_DECLSPEC ShaderModule : public Inherit<Object, ShaderModule>
    {
    public:
        ShaderModule(VkShaderModule shaderModule, Device* device, Shader* shader, AllocationCallbacks* allocator = nullptr);

        using Result = vsg::Result<ShaderModule, VkResult, VK_SUCCESS>;

        static Result create(Device* device, Shader* shader, AllocationCallbacks* allocator = nullptr);

        operator VkShaderModule() const { return _shaderModule; }

        const Shader* getShader() const { return _shader; }

        Device* getDevice() { return _device; }
        const Device* getDevice() const { return _device; }

    protected:
        virtual ~ShaderModule();

        VkShaderModule _shaderModule;

        ref_ptr<Device> _device;
        ref_ptr<Shader> _shader;
        ref_ptr<AllocationCallbacks> _allocator;
    };
    VSG_type_name(vsg::ShaderModule);

    using ShaderModules = std::vector<ref_ptr<ShaderModule>>;

} // namespace vsg
