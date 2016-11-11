#pragma once
#include "../System/device.hpp"
#include "vulkan/vkresource.hpp"

class PipelineLayout : public VkResource, public vk::PipelineLayout
{
protected:
    PipelineLayout(Device &device);

public:
    PipelineLayout() = default;
    friend void swap(PipelineLayout &p1, PipelineLayout &p2);
    PipelineLayout(PipelineLayout &&pipelineLayout);
    PipelineLayout(PipelineLayout const &pipelineLayout);
    PipelineLayout &operator=(PipelineLayout pipelineLayout);

    std::vector<vk::DescriptorSetLayout> getDescriptorSetLayouts() const;
    std::vector<vk::DescriptorSet> getDescriptorSets() const;

    ~PipelineLayout();

protected:
    std::shared_ptr<std::vector<vk::DescriptorSetLayout>> mDescriptorSetLayouts;
    std::shared_ptr<std::vector<vk::DescriptorSet>> mDescriptorSets;
};
