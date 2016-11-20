#pragma once

#include "VkTools/System/window.hpp"
#include "VkTools/System/instance.hpp"
#include "VkTools/System/device.hpp"
#include "VkTools/System/swapchain.hpp"
#include "VkTools/Memory/deviceallocator.hpp"

struct Context {
    std::shared_ptr<Window> window = std::make_shared<Window>(1024, 768);
    std::shared_ptr<Instance> instance = std::make_shared<Instance>(*window, true);
    Device device = Device(*instance);
    vk::Queue graphicsQueue = device.getGraphicQueue();

    std::shared_ptr<DeviceAllocator> deviceAllocator = std::make_shared<DeviceAllocator>(device, 1 << 26);
};
