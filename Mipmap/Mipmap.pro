TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../
LIBS += -lglfw -lvulkan -lSDL2_image -lSDL2

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
    ../VkTools/System/descriptorpool.cpp \
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
    ../VkTools/Command/transferer.cpp

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
    ../VkTools/System/descriptorpool.hpp \
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
    ../VkTools/Command/transferer.hpp
