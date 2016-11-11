#include <fstream>
#include "shadermodule.hpp"

void swap(ShaderModule &s1, ShaderModule &s2) {
    using std::swap;
    swap(static_cast<VkResource&>(s1), static_cast<VkResource&>(s2));
    std::swap(s1.m_shaderModule, s2.m_shaderModule);
}

ShaderModule::ShaderModule(ShaderModule &&shaderModule) :
    VkResource(shaderModule),
    vk::ShaderModule(shaderModule) {
    swap(*this, shaderModule);
}

ShaderModule::ShaderModule(const ShaderModule &shaderModule) :
    VkResource(shaderModule),
    vk::ShaderModule(shaderModule) {

}

ShaderModule &ShaderModule::operator =(ShaderModule shaderModule) {
    swap(*this, shaderModule);
    return *this;
}

static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios_base::ate | std::ios_base::binary);

    if(!file.is_open())
        throw std::runtime_error("failed to open file : " + filename);

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(&buffer[0], fileSize);
    return buffer;
}

ShaderModule::ShaderModule(Device &device, const std::string &path) :
    VkResource(device) {
    auto code(readFile(path));
    vk::ShaderModuleCreateInfo ci(vk::ShaderModuleCreateFlags(),
                                  code.size(), (uint32_t*)&code[0]);

    m_shaderModule = device.createShaderModule(ci);
}

ShaderModule::~ShaderModule() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroyShaderModule(m_shaderModule);
}
