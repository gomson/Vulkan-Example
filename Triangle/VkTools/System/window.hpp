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

    bool isClosed() const;
    bool isResized();

    ~Window();

private:
    GLFWwindow *mWindow;
    unsigned mWidth;
    unsigned mHeight;
    bool mIsResized = true;

    static void onWindowResized(GLFWwindow *window, int width, int height);
};
