#include "semaphore.hpp"

Semaphore::Semaphore(const Device &device) :
    VkResource(device){
    vk::SemaphoreCreateInfo info;
    m_semaphore = device.createSemaphore(info);
}

Semaphore::~Semaphore() {
    if(mDevice != nullptr && mCount != nullptr && --(*mCount) == 0)
        mDevice->destroySemaphore(m_semaphore);
}
