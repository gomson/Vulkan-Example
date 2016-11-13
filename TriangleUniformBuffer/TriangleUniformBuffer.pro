TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../
LIBS += -lglfw -lvulkan -lSDL2 -lSDL2_image

QMAKE_CXXFLAGS += -Wdeprecated

SOURCES += main.cpp \
    ../VkTools/Memory/block.cpp \
    ../VkTools/Memory/buffer.cpp \
    ../VkTools/Memory/chunk.cpp \
    ../VkTools/Memory/chunkallocator.cpp \
    ../VkTools/Memory/deviceallocator.cpp \
    ../VkTools/Memory/image.cpp \
    ../VkTools/Memory/imageloader.cpp \
    ../VkTools/Pipeline/pipeline.cpp \
    ../VkTools/Pipeline/pipelinelayout.cpp \
    ../VkTools/Pipeline/renderpass.cpp \
    ../VkTools/System/commandpool.cpp \
    ../VkTools/System/descriptorpool.cpp \
    ../VkTools/System/device.cpp \
    ../VkTools/System/fence.cpp \
    ../VkTools/System/framebuffer.cpp \
    ../VkTools/System/imageview.cpp \
    ../VkTools/System/instance.cpp \
    ../VkTools/System/sampler.cpp \
    ../VkTools/System/semaphore.cpp \
    ../VkTools/System/shadermodule.cpp \
    ../VkTools/System/swapchain.cpp \
    ../VkTools/System/window.cpp \
    ../vulkan/counter.cpp \
    ../vulkan/vkresource.cpp \
    ../VkTools/Memory/buffertransferer.cpp \
    ../VkTools/Memory/imagetransferer.cpp \
    ../VkTools/System/commandbuffersubmitter.cpp

HEADERS += \
    ../VkTools/Memory/abstractallocator.hpp \
    ../VkTools/Memory/block.hpp \
    ../VkTools/Memory/buffer.hpp \
    ../VkTools/Memory/chunk.hpp \
    ../VkTools/Memory/chunkallocator.hpp \
    ../VkTools/Memory/deviceallocator.hpp \
    ../VkTools/Memory/image.hpp \
    ../VkTools/Memory/imageloader.hpp \
    ../VkTools/Pipeline/pipeline.hpp \
    ../VkTools/Pipeline/pipelinelayout.hpp \
    ../VkTools/Pipeline/renderpass.hpp \
    ../VkTools/System/commandpool.hpp \
    ../VkTools/System/descriptorpool.hpp \
    ../VkTools/System/device.hpp \
    ../VkTools/System/fence.hpp \
    ../VkTools/System/framebuffer.hpp \
    ../VkTools/System/imageview.hpp \
    ../VkTools/System/instance.hpp \
    ../VkTools/System/sampler.hpp \
    ../VkTools/System/semaphore.hpp \
    ../VkTools/System/shadermodule.hpp \
    ../VkTools/System/swapchain.hpp \
    ../VkTools/System/window.hpp \
    ../vulkan/counter.hpp \
    ../vulkan/vkresource.hpp \
    ../vulkan/vulkan.hpp \
    ../VkTools/Memory/buffertransferer.hpp \
    ../VkTools/Memory/imagetransferer.hpp \
    ../VkTools/System/commandbuffersubmitter.hpp
