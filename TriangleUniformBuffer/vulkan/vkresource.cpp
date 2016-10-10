#include "vkresource.hpp"

void swap(VkResource &r1, VkResource &r2) {
    using std::swap;
    swap(r1.mCount, r2.mCount);
    swap(r1.mDevice, r2.mDevice);
}

VkResource::VkResource(VkResource &&resource) {
    swap(*this, resource);
}

VkResource::VkResource(VkResource const &resource) :
    mDevice(resource.mDevice),
    mCount(resource.mCount) {
    ++(*mCount);
}

VkResource::VkResource(vk::Device device) : 
    mDevice(device),
    mCount(std::make_shared<uint32_t>(1)) {

}


VkResource::~VkResource() {

}
