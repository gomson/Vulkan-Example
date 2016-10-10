#pragma once
#include "vulkan/vulkan.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Instance : public vk::Instance, private NotCopyable
{
public:
    Instance(GLFWwindow *win, bool debug);

    operator vk::Instance();

    vk::SurfaceKHR getSurfaceKHR();

    std::vector<char const *> const &getExtensions() const;

    ~Instance();

private:
    bool mDebug;
    VkDebugReportCallbackEXT mCallback;
    std::vector<char const *> mExtensions;
    std::vector<char const *> mLayers;
    vk::SurfaceKHR mSurfaceKHR;
};
