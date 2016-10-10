#include "semaphore.hpp"

void swap(Semaphore &s1, Semaphore &s2) {
    using std::swap;
    swap(static_cast<VkResource&>(s1), static_cast<VkResource&>(s2));
    swap(s1.m_semaphore, s2.m_semaphore);
}

Semaphore::Semaphore(Semaphore &&semaphore) :
    VkResource(std::move(semaphore)),
    vk::Semaphore(semaphore) {
    swap(*this, semaphore);
}

Semaphore::Semaphore(const Semaphore &semaphore) :
    VkResource(semaphore),
    vk::Semaphore(semaphore) {

}

Semaphore &Semaphore::operator =(Semaphore semaphore) {
    swap(*this, semaphore);
    return *this;
}

Semaphore::Semaphore(vk::Device const &device) :
    VkResource(device){
    vk::SemaphoreCreateInfo info;
    m_semaphore = device.createSemaphore(info);
}

Semaphore::~Semaphore() {
    if(mCount != nullptr && --(*mCount) == 0)
        mDevice.destroySemaphore(m_semaphore);
}
