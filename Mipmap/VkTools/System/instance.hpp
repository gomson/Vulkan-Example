#pragma once
#include "vulkan/vulkan.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Instance : public vk::Instance, private NotCopyable
{
public:
    Instance(GLFWwindow *win, bool debug);

    vk::SurfaceKHR getSurfaceKHR();

    std::vector<char const *> const &getExtensions() const;

    ~Instance();

private:
    VkDebugReportCallbackEXT mCallback = VK_NULL_HANDLE;
    std::vector<char const *> mExtensions;
    std::vector<char const *> mLayers;
    vk::SurfaceKHR mSurfaceKHR;

    static uint32_t numberInstances;
};
