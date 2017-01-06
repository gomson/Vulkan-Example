#include "staticgeometrystep.hpp"

StaticGeometryStep::StaticGeometryStep(Device const &device, RenderingPass &renderpass) :
    mDevice(std::make_shared<Device>(device)),
    mPipeline(std::make_shared<StaticGeometryPipeline>(device, "../Shaders/geometry_static", renderpass)) {

}
