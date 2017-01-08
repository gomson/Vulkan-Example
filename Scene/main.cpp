#include <iostream>
#include <memory>
#include <utility>
#include "Renderer/context.hpp"
#include "Renderer/Step/presentationstep.hpp"
#include "Renderer/Step/renderingstep.hpp"
#include "VkTools/Command/commandbuffersubmitter.hpp"
#include "VkTools/Memory/deviceallocator.hpp"

int main()
{
    Context context;

    CommandPool transientCommandBufferPool(context.device, true, true, context.device.getIndexFamillyQueue());
    CommandPool drawCommandBufferPool(context.device, false, false, context.device.getIndexFamillyQueue());
    CommandBufferSubmitter commandBufferSubmitter(context.device, transientCommandBufferPool, 100);
    auto deviceAllocator(std::make_shared<DeviceAllocator>(context.device, 1 << 25));
    Transferer transferer(1, 1 << 20, deviceAllocator, commandBufferSubmitter);
    PresentationStep presentationStep(context.device,
                                      commandBufferSubmitter,
                                      drawCommandBufferPool,
                                      context.graphicsQueue, transferer);

    Window &window = *context.window;
    RenderingStep renderingStep(context.device, commandBufferSubmitter, transferer);

    ModelLoader modelLoader;

    ModelLoader_t cube = modelLoader.open("../Models/Sponza/sponza.obj", renderingStep.getRefStaticVbo(), renderingStep.getRefStaticIbo(), renderingStep.getRefNumberStaticIndices(), renderingStep.getRefNumberStaticVertices(), transferer, renderingStep.getRefMaterialDescriptorSetManager());

    renderingStep.setRootNode(std::make_shared<StaticNode>(cube));

    while(!window.isClosed()) {
        glfwPollEvents();

        if(window.isResized()) {
            if(window.isSurfaceKHROutOfDate()) {
                presentationStep.destroySwapchainKHR();
                context.instance->createSurfaceKHR();
            }
            presentationStep.rebuildSwapchainKHR(context.instance->getSurfaceKHR());
            commandBufferSubmitter.setNumberBatch(presentationStep.getNumberImages());
            renderingStep.resize(window.getWidth(), window.getHeight(), deviceAllocator, presentationStep.getNumberImages());
            presentationStep.updateImages(renderingStep.getFramebuffer(), 0);
            commandBufferSubmitter.wait();
            commandBufferSubmitter.submit();
            context.device.waitIdle();// big synchro
        }

        try {
            uint32_t index = presentationStep.getCurrentIndex();
            renderingStep.execute(index);
            presentationStep.execute(index);
        }

        catch(std::system_error const &exception) {
            if(exception.code() == vk::Result::eErrorOutOfDateKHR)
                window.surfaceIsOutOfDate();
        }
    }

    context.device.waitIdle(); // We wait for the end of all operations for destroy all objects

    return 0;
}
