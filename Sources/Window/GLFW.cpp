#include "GLFW.hpp"
#include <GLFW/glfw3.h>

namespace Engine::Window {

    // All the static stuff init
    double GLFW_Window_wrapper::_x { 0 };
    double GLFW_Window_wrapper::_y { 0 };

    // Constructor and Destructor, helper function

    bool GLFW_Window_wrapper::InitGLFW(int w, int h, const std::string& title) {
        /* Initialize the library */
        if (!glfwInit()) {
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        WindowHandle* window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);

        if (!window) {
            return false;
        }

        windowHandle = window;
        isCreated = true;
        glfwSetWindowUserPointer(window, this);
        return true;
    }

    GLFW_Window_wrapper::GLFW_Window_wrapper(int w, int h, const std::string& title) try :
        isCreated(false), windowHandle(nullptr) {

        if (!InitGLFW(w, h, title)) throw std::runtime_error("GLFW initialization Error");

        // hook up callbacks
        glfwSetCharCallback(windowHandle, GLFW_Window_wrapper::char_callback);
        glfwSetScrollCallback(windowHandle, GLFW_Window_wrapper::scroll_callback);
        glfwSetFramebufferSizeCallback(windowHandle, GLFW_Window_wrapper::framebuffer_resize_callback);

    } catch (const std::exception&) {}


    GLFW_Window_wrapper::~GLFW_Window_wrapper() {
        glfwTerminate();
    }


    // Other functions

    std::vector<const char*> GLFW_Window_wrapper::GetGLFWRequiredInstanceExtensions() {
        auto glfwExtCount = 0u;
        auto glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);
        return {glfwExt, glfwExt + glfwExtCount};
    }

    WindowHandle* GLFW_Window_wrapper::GetHandle() {
        return windowHandle;
    }

    bool GLFW_Window_wrapper::KeepWindowOpen() {
        return !static_cast<bool>(glfwWindowShouldClose(windowHandle));
    }

    void GLFW_Window_wrapper::PollEvents() {
        glfwPollEvents();
    }

    void GLFW_Window_wrapper::WaitEvents() {
        glfwWaitEvents();
    }

    void Engine::Window::GLFW_Window_wrapper::SetWindowShouldClose() {
        glfwSetWindowShouldClose(windowHandle, 1);
    }

    void GLFW_Window_wrapper::WaitEventsTimeout(double t) {
        glfwWaitEventsTimeout(t);
    }

    void Engine::Window::GLFW_Window_wrapper::UpdateTitle(const std::string& new_title) {
        glfwSetWindowTitle(windowHandle, new_title.c_str());
    }


    // Callbacks

    void GLFW_Window_wrapper::ProcessTextInput(unsigned int codepoint) {
    }

    void GLFW_Window_wrapper::ProcessScroll(double x, double y) {
        //GLFW_Window_wrapper::_x += x;
        //GLFW_Window_wrapper::_y += y;
        //std::cerr << "\r                                                       "
        //    << std::setprecision(4)
        //    << "\r(X, Y): " << GLFW_Window_wrapper::_x << ", " << GLFW_Window_wrapper::_y;
    }

    void GLFW_Window_wrapper::WindowResized(int new_width, int new_height) {
    }

    void GLFW_Window_wrapper::WindowLoop() {
    }

    void GLFW_Window_wrapper::SwapBuffers() {
    }


    // ** C to C++ functions callbacks mapping **


    void GLFW_Window_wrapper::scroll_callback(WindowHandle* wd, double xoffset, double yoffset) {
        GetInstance(wd)->ProcessScroll(xoffset, yoffset);
    }

    void GLFW_Window_wrapper::char_callback(WindowHandle* wd, unsigned int codepoint) {
        GetInstance(wd)->ProcessTextInput(codepoint);
    }

    void GLFW_Window_wrapper::framebuffer_resize_callback(WindowHandle* wd, int w, int h) {
        GetInstance(wd)->WindowResized(w, h);
    }

    GLFW_Window_wrapper* GLFW_Window_wrapper::GetInstance(WindowHandle* handle) {
        return reinterpret_cast<GLFW_Window_wrapper*>(glfwGetWindowUserPointer(handle));
    }


    //
    //
}
