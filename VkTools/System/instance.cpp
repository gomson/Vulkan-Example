#include <iostream>
#include <cassert>
#include "instance.hpp"
#include <SDL2/SDL_image.h>

uint32_t Instance::numberInstances = 0;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT,
                                                    VkDebugReportObjectTypeEXT,
                                                    uint64_t,
                                                    size_t,
                                                    int32_t,
                                                    const char*,
                                                    const char* msg,
                                                    void*) {
    std::cerr << "validation layer: " << msg << std::endl;
    return VK_FALSE;
}

Instance::Instance(GLFWwindow *window, bool debug) : mWindow(window) {
    vk::ApplicationInfo applicationInfo;

    applicationInfo.setPApplicationName("VkTools").setPEngineName("Lava");
    applicationInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0)).setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 26);

    vk::InstanceCreateInfo createInfo;

    uint32_t nExtensions;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&nExtensions);

    std::cout << "Extensions chose :" << std::endl;
    for(uint32_t i = 0; i < nExtensions; ++i) {
        mExtensions.push_back(glfwExtensions[i]);
        std::cout << "    " << glfwExtensions[i] << std::endl;
    }

    std::vector<vk::LayerProperties> layersProperty = vk::enumerateInstanceLayerProperties();
    if(debug) {
        std::cout << "Layers chose :" << std::endl;
        for(vk::LayerProperties &lp : layersProperty) {
            if(!strcmp(lp.layerName,"VK_LAYER_LUNARG_standard_validation")) {
                mLayers.push_back(lp.layerName);
                std::cout << "    " << lp.layerName << " :" << std::endl;
            }
        }

        mExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        createInfo.setPpEnabledLayerNames(mLayers.data());
    }

    createInfo.setPApplicationInfo(&applicationInfo);
    createInfo.setPpEnabledExtensionNames(mExtensions.data()).setEnabledExtensionCount(mExtensions.size());
    createInfo.setEnabledLayerCount(mLayers.size());

    m_instance = vk::createInstance(createInfo);

    if(debug) {
        vk::DebugReportCallbackCreateInfoEXT debugInfo;
        debugInfo.setFlags(vk::DebugReportFlagBitsEXT::eDebug | /*vk::DebugReportFlagBitsEXT::eInformation | */
                        //   vk::DebugReportFlagBitsEXT::eError |
                           vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                           vk::DebugReportFlagBitsEXT::eWarning).setPfnCallback(debugCallback);

        auto func = (PFN_vkCreateDebugReportCallbackEXT)getProcAddr("vkCreateDebugReportCallbackEXT");
        func(m_instance, &debugInfo.operator const VkDebugReportCallbackCreateInfoEXT &(), nullptr, &mCallback);
    }

    createSurfaceKHR();

    if(numberInstances++ == 0)
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
}

vk::SurfaceKHR Instance::getSurfaceKHR() {
    return mSurfaceKHR;
}

std::vector<char const*> const &Instance::getExtensions() const {
    return mExtensions;
}

void Instance::createSurfaceKHR() {
    if(mSurfaceKHR != vk::SurfaceKHR())
        destroySurfaceKHR(mSurfaceKHR);
    VkSurfaceKHR surface;
    assert(glfwCreateWindowSurface(m_instance, mWindow, nullptr, &surface) == VK_SUCCESS);
    mSurfaceKHR = surface;
}

Instance::~Instance() {
    if(mCallback != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)getProcAddr("vkDestroyDebugReportCallbackEXT");
        func(m_instance, mCallback, nullptr);
    }
    destroySurfaceKHR(mSurfaceKHR);
    destroy();

    if(--numberInstances == 0)
        IMG_Quit();
}