#pragma once
#include "VkTools/Pipeline/renderpass.hpp"

// A renderPass to render into our swapchain
class RenderPassToPresent : public RenderPass {
public:
    RenderPassToPresent(Device const &device);
};
