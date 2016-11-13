#include "vkresource.hpp"

VkResource::VkResource(const Device &device) :
    mDevice(std::make_shared<Device>(device)) {

}

vk::Device VkResource::getDevice() const {
    return *mDevice;
}
