#pragma once
#include <string>
struct GLFWwindow;


class Window
{
private:
    GLFWwindow* window_ = nullptr;

public:
    Window(int width, int height, const std::string& title);
    ~Window();

    GLFWwindow* raw() const { return window_; }
    bool shouldClose() const;
    void swapAndPoll() const;
};
