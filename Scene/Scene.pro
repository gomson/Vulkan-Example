TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../
LIBS += -lglfw -lvulkan -lSDL2_image -lSDL2 -lassimp

SOURCES += main.cpp \
    ../VkTools/Image/framebuffer.cpp \
    ../VkTools/Image/image.cpp \
    ../VkTools/Image/imageloader.cpp \
    ../VkTools/Image/imageview.cpp \
    ../VkTools/Image/sampler.cpp \
    ../VkTools/Memory/block.cpp \
    ../VkTools/Memory/chunk.cpp \
    ../VkTools/Memory/chunkallocator.cpp \
    ../VkTools/Memory/deviceallocator.cpp \
    ../VkTools/Pipeline/pipeline.cpp \
    ../VkTools/Pipeline/pipelinelayout.cpp \
    ../VkTools/Pipeline/renderpass.cpp \
    ../VkTools/Synchronization/fence.cpp \
    ../VkTools/Synchronization/semaphore.cpp \
    ../VkTools/System/device.cpp \
    ../VkTools/System/instance.cpp \
    ../VkTools/System/shadermodule.cpp \
    ../VkTools/System/swapchain.cpp \
    ../VkTools/System/window.cpp \
    ../vulkan/counter.cpp \
    ../vulkan/vkresource.cpp \
    ../VkTools/Buffer/buffer.cpp \
    ../VkTools/Command/commandbuffersubmitter.cpp \
    ../VkTools/Command/commandpool.cpp \
    ../VkTools/Command/transferer.cpp \
    Renderer/Step/presentationstep.cpp \
    Renderer/RenderPass/renderpasstopresent.cpp \
    Renderer/Pipeline/pipelinetopresent.cpp \
    Renderer/PipelineLayout/pipelinelayouttopresent.cpp \
    ../VkTools/Descriptor/descriptorsetlayout.cpp \
    ../VkTools/Image/completeframebuffer.cpp \
    ../VkTools/Descriptor/descriptorpool.cpp \
    Renderer/Step/renderingstep.cpp \
    Renderer/RenderPass/renderingpass.cpp \
    Renderer/FrameBuffer/renderingframebuffer.cpp \
    Renderer/Pipeline/staticgeometrypipeline.cpp \
    Renderer/PipelineLayout/staticgeometrypipelinelayout.cpp \
    Renderer/Step/staticgeometrystep.cpp \
    Renderer/Model/mesh.cpp \
    Renderer/Model/modelimporter.cpp \
    Renderer/Model/modelloader.cpp \
    Renderer/Scene/staticnode.cpp \
    Renderer/Material/materialdescriptorsetpool.cpp \
    Renderer/Model/material.cpp

HEADERS += \
    ../VkTools/Image/framebuffer.hpp \
    ../VkTools/Image/image.hpp \
    ../VkTools/Image/imageloader.hpp \
    ../VkTools/Image/imageview.hpp \
    ../VkTools/Image/sampler.hpp \
    ../VkTools/Memory/abstractallocator.hpp \
    ../VkTools/Memory/block.hpp \
    ../VkTools/Memory/chunk.hpp \
    ../VkTools/Memory/chunkallocator.hpp \
    ../VkTools/Memory/deviceallocator.hpp \
    ../VkTools/Pipeline/pipeline.hpp \
    ../VkTools/Pipeline/pipelinelayout.hpp \
    ../VkTools/Pipeline/renderpass.hpp \
    ../VkTools/Synchronization/fence.hpp \
    ../VkTools/Synchronization/semaphore.hpp \
    ../VkTools/System/device.hpp \
    ../VkTools/System/instance.hpp \
    ../VkTools/System/shadermodule.hpp \
    ../VkTools/System/swapchain.hpp \
    ../VkTools/System/window.hpp \
    ../vulkan/counter.hpp \
    ../vulkan/vkresource.hpp \
    ../vulkan/vulkan.hpp \
    ../VkTools/Buffer/buffer.hpp \
    ../VkTools/Command/commandbuffersubmitter.hpp \
    ../VkTools/Command/commandpool.hpp \
    ../VkTools/Command/transferer.hpp \
    Renderer/context.hpp \
    Renderer/Step/presentationstep.hpp \
    Renderer/RenderPass/renderpasstopresent.hpp \
    Renderer/Pipeline/pipelinetopresent.hpp \
    Renderer/PipelineLayout/pipelinelayouttopresent.hpp \
    ../VkTools/Descriptor/descriptorsetlayout.hpp \
    ../VkTools/Image/completeframebuffer.hpp \
    ../VkTools/Descriptor/descriptorpool.hpp \
    Renderer/Step/renderingstep.hpp \
    Renderer/RenderPass/renderingpass.hpp \
    Renderer/FrameBuffer/renderingframebuffer.hpp \
    Renderer/PipelineLayout/staticgeometrypipelinelayout.hpp \
    Renderer/Pipeline/staticgeometrypipeline.hpp \
    Renderer/Step/staticgeometrystep.hpp \
    Renderer/Model/mesh.hpp \
    Renderer/Model/modelimporter.hpp \
    Renderer/Model/modelloader.hpp \
    Renderer/Scene/staticnode.hpp \
    Renderer/Scene/node.hpp \
    Renderer/Material/materialdescriptorsetpool.hpp \
    Renderer/Model/material.hpp \
    ../VkTools/Image/texture.hpp
