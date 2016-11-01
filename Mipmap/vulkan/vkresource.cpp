#include "vkresource.hpp"

VkResource::VkResource(Device device) :
    mDevice(device) {

}

Device VkResource::getDevice() const {
    return mDevice;
}

VkResource &VkResource::operator =(VkResource vkResource) {
    using std::swap;
    swap(static_cast<Counter&>(*this), static_cast<Counter&>(vkResource));
    swap(mDevice, vkResource.mDevice);
    return *this;
}
