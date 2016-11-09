TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lglfw -lvulkan -lSDL2_image -lSDL2

SOURCES += main.cpp \
    VkTools/System/window.cpp \
    VkTools/System/instance.cpp \
    VkTools/System/device.cpp \
    VkTools/System/swapchain.cpp \
    VkTools/System/imageview.cpp \
    VkTools/System/shadermodule.cpp \
    VkTools/System/framebuffer.cpp \
    vulkan/vkresource.cpp \
    VkTools/System/semaphore.cpp \
    VkTools/System/fence.cpp \
    VkTools/System/commandpool.cpp \
    VkTools/Memory/buffer.cpp \
    VkTools/Memory/chunkallocator.cpp \
    VkTools/Memory/chunk.cpp \
    VkTools/Memory/block.cpp \
    VkTools/Memory/deviceallocator.cpp \
    VkTools/Memory/bufferimagetransferer.cpp \
    VkTools/System/descriptorpool.cpp \
    VkTools/Pipeline/pipeline.cpp \
    VkTools/Pipeline/renderpass.cpp \
    VkTools/Pipeline/pipelinelayout.cpp \
    vulkan/counter.cpp \
    VkTools/Memory/image.cpp \
    VkTools/Memory/imageloader.cpp \
    VkTools/System/sampler.cpp

HEADERS += \
    VkTools/System/window.hpp \
    VkTools/System/instance.hpp \
    VkTools/System/device.hpp \
    vulkan/vulkan.hpp \
    VkTools/System/swapchain.hpp \
    VkTools/System/imageview.hpp \
    VkTools/System/shadermodule.hpp \
    VkTools/Pipeline/abstractpipelineshaderstage.hpp \
    VkTools/System/framebuffer.hpp \
    vulkan/vkresource.hpp \
    VkTools/System/semaphore.hpp \
    VkTools/System/fence.hpp \
    VkTools/System/commandpool.hpp \
    VkTools/Memory/buffer.hpp \
    VkTools/Memory/chunkallocator.hpp \
    VkTools/Memory/chunk.hpp \
    VkTools/Memory/abstractallocator.hpp \
    VkTools/Memory/block.hpp \
    VkTools/Memory/deviceallocator.hpp \
    VkTools/Memory/bufferimagetransferer.hpp \
    VkTools/System/descriptorpool.hpp \
    VkTools/Pipeline/renderpass.hpp \
    VkTools/Pipeline/pipelinelayout.hpp \
    VkTools/Pipeline/pipeline.hpp \
    vulkan/counter.hpp \
    VkTools/Memory/image.hpp \
    VkTools/Memory/imageloader.hpp \
    VkTools/System/sampler.hpp
