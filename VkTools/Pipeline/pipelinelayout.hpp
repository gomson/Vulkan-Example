#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class PipelineLayout : public VkResource, public vk::PipelineLayout
{
public:
    PipelineLayout(Device const &device);

public:
    PipelineLayout() = default;
    PipelineLayout(PipelineLayout &&pipelineLayout) = default;
    PipelineLayout(PipelineLayout const &pipelineLayout) = default;
    PipelineLayout &operator=(PipelineLayout pipelineLayout);

    std::vector<vk::DescriptorSetLayout> getDescriptorSetLayouts() const;
    std::vector<vk::DescriptorSet> getDescriptorSets() const;

    ~PipelineLayout();

protected:
    std::shared_ptr<std::vector<vk::DescriptorSetLayout>> mDescriptorSetLayouts;
    std::shared_ptr<std::vector<vk::DescriptorSet>> mDescriptorSets;
};
