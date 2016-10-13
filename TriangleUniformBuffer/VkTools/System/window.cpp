#include "window.hpp"

Window::Window(unsigned width, unsigned height) :
    mWidth(width), mHeight(height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(width, height, "Vulkan test", nullptr, nullptr);
    // set this
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetWindowSizeCallback(mWindow, Window::onWindowResized);
}

unsigned Window::getWidth() const {
    return mWidth;
}

unsigned Window::getHeight() const {
    return mHeight;
}

bool Window::isClosed() const {
    return glfwWindowShouldClose(mWindow);
}

bool Window::isResized() {
    auto result = mIsResized;
    mIsResized = false;
    return result;
}

Window::operator GLFWwindow *() {
    return mWindow;
}

void Window::onWindowResized(GLFWwindow *window, int width, int height) {
    if(width == 0 || height == 0)
        return;

    // Get "this"
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->mWidth = width;
    app->mHeight = height;
    app->mIsResized = true;
}

Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}