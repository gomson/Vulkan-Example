#pragma once
#include "VkTools/System/device.hpp"

class AbstractRenderingStep
{
public:
    AbstractRenderingStep(Device const &device) :
        mDevice(std::make_shared<Device>(device)){}

    virtual void execute() = 0;

    virtual ~AbstractRenderingStep() = default;

protected:
    std::shared_ptr<Device> mDevice;
};
