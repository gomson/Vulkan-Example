#include "geometrypipeline.hpp"
#include "../PipelineLayout/geometrypipelinelayout.hpp"

GeometryPipeline::GeometryPipeline(const Device &device, std::string shaderPrefix, const GeometryRenderPass &renderPass) :
    Pipeline(device, GeometryPipelineLayout(device)) {
    mShaderModules->emplace_back(ShaderModule(device, shaderPrefix + "_vert.spv"));
    mShaderModules->emplace_back(ShaderModule(device, shaderPrefix + "_frag.spv"));
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStage;
    vk::PipelineShaderStageCreateInfo shaderStageInfo;
    shaderStageInfo.pName = "main";
    shaderStageInfo.module = (*mShaderModules)[0];
    shaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    shaderStage.emplace_back(shaderStageInfo);
    shaderStageInfo.module = (*mShaderModules)[1];
    shaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    shaderStage.emplace_back(shaderStageInfo);

    vk::VertexInputBindingDescription binding(0, sizeof(Vertex));
    std::vector<vk::VertexInputAttributeDescription> attributes;
    attributes.emplace_back(0, 0, vk::Format::eR32G32B32Sfloat, 0);
    attributes.emplace_back(1, 0, vk::Format::eR32G32Sfloat, sizeof(glm::vec3));
    attributes.emplace_back(2, 0, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec2) + sizeof(glm::vec3));
    attributes.emplace_back(3, 0, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec2) + 2 * sizeof(glm::vec3));
    vk::PipelineVertexInputStateCreateInfo vertexInput(vk::PipelineVertexInputStateCreateFlags(),
                                                       1, &binding, attributes.size(), attributes.data());

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(vk::PipelineInputAssemblyStateCreateFlags(),
                                                           vk::PrimitiveTopology::eTriangleStrip);

    vk::PipelineViewportStateCreateInfo viewport(vk::PipelineViewportStateCreateFlags(),
                                                 1, nullptr, 1, nullptr);

    vk::PipelineRasterizationStateCreateInfo rasterization(vk::PipelineRasterizationStateCreateFlags(),
                                                          0, false, vk::PolygonMode::eFill,
                                                           vk::CullModeFlagBits::eNone,
                                                           vk::FrontFace::eCounterClockwise,
                                                           false, 0.0, 0.0, 0.0, 1.0);

    vk::PipelineDepthStencilStateCreateInfo depthStencil(vk::PipelineDepthStencilStateCreateFlags(),
                                                         true, true, vk::CompareOp::eLess);

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
                                        &depthStencil, &colorBlend, &dynamic, *mPipelineLayout,
                                        renderPass, 0);
    m_pipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, info);
}
