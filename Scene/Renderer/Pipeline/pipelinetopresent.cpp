#include "pipelinetopresent.hpp"
#include "../PipelineLayout/pipelinelayouttopresent.hpp"

PipelineToPresent::PipelineToPresent(const Device &device, RenderPass renderPass) :
    Pipeline(device, PipelineLayoutToPresent(device))
{
    mShaderModules->emplace_back(ShaderModule(device, "../Shaders/final_vert.spv"));
    mShaderModules->emplace_back(ShaderModule(device, "../Shaders/final_frag.spv"));
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStage;
    vk::PipelineShaderStageCreateInfo shaderStageInfo;
    shaderStageInfo.pName = "main";
    shaderStageInfo.module = (*mShaderModules)[0];
    shaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    shaderStage.emplace_back(shaderStageInfo);
    shaderStageInfo.module = (*mShaderModules)[1];
    shaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    shaderStage.emplace_back(shaderStageInfo);

    vk::VertexInputBindingDescription binding(0, sizeof(glm::vec2));
    vk::VertexInputAttributeDescription attribute(0, 0, vk::Format::eR32G32Sfloat);
    vk::PipelineVertexInputStateCreateInfo vertexInput(vk::PipelineVertexInputStateCreateFlags(),
                                                       1, &binding, 1, &attribute);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(vk::PipelineInputAssemblyStateCreateFlags(),
                                                           vk::PrimitiveTopology::eTriangleStrip);

    vk::PipelineViewportStateCreateInfo viewport(vk::PipelineViewportStateCreateFlags(),
                                                 1, nullptr, 1, nullptr);

    vk::PipelineRasterizationStateCreateInfo rasterization(vk::PipelineRasterizationStateCreateFlags(),
                                                          0, false, vk::PolygonMode::eFill,
                                                           vk::CullModeFlagBits::eNone,
                                                           vk::FrontFace::eCounterClockwise,
                                                           false, 0.0, 0.0, 0.0, 1.0);

    vk::PipelineMultisampleStateCreateInfo multisample;
    vk::PipelineColorBlendAttachmentState colorBlendAttachment(false);
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eA |
                                          vk::ColorComponentFlagBits::eB |
                                          vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eR;

    vk::PipelineColorBlendStateCreateInfo colorBlend(vk::PipelineColorBlendStateCreateFlags(),
                                                     false, vk::LogicOp(),
                                                     1, &colorBlendAttachment);

    vk::DynamicState dynamics[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic(vk::PipelineDynamicStateCreateFlags(),
                                               2, dynamics);

    vk::GraphicsPipelineCreateInfo info(vk::PipelineCreateFlags(),
                                        shaderStage.size(), shaderStage.data(),
                                        &vertexInput, &inputAssembly, nullptr,
                                        &viewport, &rasterization, &multisample,
                                        nullptr, &colorBlend, &dynamic, *mPipelineLayout,
                                        renderPass, 0);
    m_pipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, info);
}
