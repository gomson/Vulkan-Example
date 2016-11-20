#include "pipelinelayouttopresent.hpp"

PipelineLayoutToPresent::PipelineLayoutToPresent(Device const &device) :
    PipelineLayout(device) {
    vk::PipelineLayoutCreateInfo info(vk::PipelineLayoutCreateFlags(),
                                      0, nullptr,
                                      0, nullptr);
    m_pipelineLayout = device.createPipelineLayout(info);
}
