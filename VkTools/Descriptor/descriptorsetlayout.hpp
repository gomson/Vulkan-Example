#pragma once
#include <unordered_map>
#include "vulkan/vkresource.hpp"

struct DescriptorTypeHash {
    std::size_t operator()(vk::DescriptorType const &dt) const {
        return std::hash<uint32_t>()((uint32_t)dt);
    }
};

class DescriptorSetLayout : public VkResource, public vk::DescriptorSetLayout
{
public:
    DescriptorSetLayout(const Device &device, vk::DescriptorSetLayoutCreateInfo info);

    DescriptorSetLayout(DescriptorSetLayout &&descriptorSetLayout) = default;
    DescriptorSetLayout(DescriptorSetLayout const &descriptorSetLayout) = default;
    DescriptorSetLayout &operator=(DescriptorSetLayout descriptorSetLayout);

    std::unordered_map<vk::DescriptorType, unsigned, DescriptorTypeHash> getDescriptorTypes() const;

    ~DescriptorSetLayout();

private:
    std::shared_ptr<std::unordered_map<vk::DescriptorType, unsigned, DescriptorTypeHash>> mDescriptorTypes;
};
