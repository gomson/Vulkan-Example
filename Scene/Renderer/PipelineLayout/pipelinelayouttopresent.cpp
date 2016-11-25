#include "pipelinelayouttopresent.hpp"

PipelineLayoutToPresent::PipelineLayoutToPresent(Device const &device, std::shared_ptr<Sampler> sampler) :
    PipelineLayout(device), mSampler(sampler) {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    vk::Sampler s = *mSampler;
    bindings.emplace_back(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, &s);

    mDescriptorSetLayouts->emplace_back(device, vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(),
                                                                                  bindings.size(), bindings.data()));
    vk::PipelineLayoutCreateInfo info(vk::PipelineLayoutCreateFlags(),
                                      mDescriptorSetLayouts->size(), mDescriptorSetLayouts->data(),
                                      0, nullptr);
    m_pipelineLayout = device.createPipelineLayout(info);
}
