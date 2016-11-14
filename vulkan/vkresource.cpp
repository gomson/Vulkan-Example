#include "vkresource.hpp"

VkResource::VkResource(const Device &device) :
    mDevice(std::make_shared<Device>(device)) {

}

Device VkResource::getDevice() const {
    return *mDevice;
}
