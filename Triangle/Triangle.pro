TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lglfw -lvulkan

SOURCES += main.cpp \
    VkTools/System/window.cpp \
    VkTools/System/instance.cpp \
    VkTools/System/device.cpp \
    VkTools/System/swapchain.cpp \
    VkTools/System/imageview.cpp \
    VkTools/System/shadermodule.cpp \
    VkTools/Pipeline/abstractpipeline.cpp \
    VkTools/System/framebuffer.cpp \
    VkTools/Pipeline/abstractrenderpass.cpp \
    VkTools/Pipeline/abstractpipelinelayout.cpp \
    vulkan/vkresource.cpp \
    VkTools/System/semaphore.cpp \
    VkTools/System/fence.cpp \
    VkTools/System/commandpool.cpp

HEADERS += \
    VkTools/System/window.hpp \
    VkTools/System/instance.hpp \
    VkTools/System/device.hpp \
    vulkan/vulkan.hpp \
    VkTools/System/swapchain.hpp \
    VkTools/System/imageview.hpp \
    VkTools/System/shadermodule.hpp \
    VkTools/Pipeline/abstractpipelineshaderstage.hpp \
    VkTools/Pipeline/abstractpipeline.hpp \
    VkTools/System/framebuffer.hpp \
    VkTools/Pipeline/abstractrenderpass.hpp \
    VkTools/Pipeline/abstractpipelinelayout.hpp \
    vulkan/vkresource.hpp \
    VkTools/System/semaphore.hpp \
    VkTools/System/fence.hpp \
    VkTools/System/commandpool.hpp
