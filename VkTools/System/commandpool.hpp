#pragma once
#include "vulkan/vkresource.hpp"

class CommandPool : public VkResource, public vk::CommandPool
{
public:
    /**
     * @brief CommandPool
     * @param device
     * @param transient : Are command buffer will be short lived
     * @param resetableBuffer : Are command buffer invidually resetable
     * @param queueFamillyIndex
     */
    CommandPool(Device &device, bool transient, bool resetableBuffer,
                uint32_t queueFamillyIndex);
    
    CommandPool(CommandPool &&commandPool) = default;
    CommandPool(CommandPool const &commandPool) = default;
    CommandPool &operator=(CommandPool const &commandPool) = default;

    std::vector<vk::CommandBuffer> allocate(vk::CommandBufferLevel level, uint32_t count);
    void reset(bool releaseResources);

    ~CommandPool();
};
