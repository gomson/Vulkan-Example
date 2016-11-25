#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"
#include "../Descriptor/descriptorsetlayout.hpp"

class PipelineLayout : public VkResource, public vk::PipelineLayout
{
public:
    PipelineLayout(Device const &device);

public:
    PipelineLayout() = default;
    PipelineLayout(PipelineLayout &&pipelineLayout) = default;
    PipelineLayout(PipelineLayout const &pipelineLayout) = default;
    PipelineLayout &operator=(PipelineLayout pipelineLayout);

    std::vector<DescriptorSetLayout> getDescriptorSetLayouts() const;

    ~PipelineLayout();

protected:
    std::shared_ptr<std::vector<DescriptorSetLayout>> mDescriptorSetLayouts = std::make_shared<std::vector<DescriptorSetLayout>>();
};
