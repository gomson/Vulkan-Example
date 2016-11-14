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

bool Window::isClosed() {
    double currentTime = glfwGetTime();
    ++mFrame;
    if(currentTime - mLastFrame >= 1.0) {
        mFps = mFrame;
        mLastFrame += 1.0;
        mFrame = 0;
    }
    return glfwWindowShouldClose(mWindow);
}

uint32_t Window::getFps() const {
    return mFps;
}

bool Window::isResized() {
    auto result = mIsResized;
    mIsResized = false;
    return result;
}

bool Window::isSurfaceKHROutOfDate() {
    auto result = mIsSurfaceKHROutOfDate;
    mIsSurfaceKHROutOfDate = false;
    return result;
}

void Window::surfaceIsOutOfDate() {
    mIsSurfaceKHROutOfDate = true;
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
