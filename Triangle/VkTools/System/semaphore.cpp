#include "semaphore.hpp"

Semaphore::Semaphore(vk::Device const &device) :
    VkResource(device){
    vk::SemaphoreCreateInfo info;
    m_semaphore = device.createSemaphore(info);
}

Semaphore::~Semaphore() {
    mDevice.destroySemaphore(m_semaphore);
}
