#include <fstream>
#include "shadermodule.hpp"

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

ShaderModule::ShaderModule(vk::Device &device, const std::string &path) :
    VkResource(device) {
    auto code(readFile(path));
    vk::ShaderModuleCreateInfo ci(vk::ShaderModuleCreateFlags(),
                                  code.size(), (uint32_t*)&code[0]);

    m_shaderModule = device.createShaderModule(ci);
}

ShaderModule::~ShaderModule() {
    mDevice.destroyShaderModule(m_shaderModule);
}
