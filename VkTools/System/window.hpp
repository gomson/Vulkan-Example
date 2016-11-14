#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
public:
    Window(unsigned width, unsigned height);

    operator GLFWwindow*();

    unsigned getWidth() const;
    unsigned getHeight() const;

    bool isClosed();
    bool isResized();
    bool isSurfaceKHROutOfDate();

    void surfaceIsOutOfDate();

    uint32_t getFps() const;

    ~Window();

private:
    GLFWwindow *mWindow;
    unsigned mWidth;
    unsigned mHeight;
    bool mIsResized = true;
    bool mIsSurfaceKHROutOfDate = true;
    double mLastFrame = glfwGetTime();
    uint32_t mFps = 0;
    uint32_t mFrame = 0;

    static void onWindowResized(GLFWwindow *window, int width, int height);
};
