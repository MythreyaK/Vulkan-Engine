#ifndef GLFW_WRAPPER_HPP
#define GLFW_WRAPPER_HPP

#include <string>
#include <iostream>
#include <vector>

extern "C" {
    struct GLFWwindow;
}

typedef GLFWwindow WindowHandle;

namespace Engine::Window {


    class GLFW_Window_wrapper {
    private:
        // Variables
        static double _x;
        static double _y;

        bool isCreated;
        WindowHandle* windowHandle;

        // Methods
        bool InitGLFW(int w, int h, const std::string& title);

        static void scroll_callback(WindowHandle* window, double xoffset, double yoffset);
        static void char_callback(WindowHandle* window, unsigned int codepoint);
        static void framebuffer_resize_callback(WindowHandle *w, int width, int height);

        static GLFW_Window_wrapper* GetInstance(WindowHandle* ptr);

    public:
        GLFW_Window_wrapper(int w = 300, int h = 300, const std::string& title = std::string("Window"));
        GLFW_Window_wrapper(GLFW_Window_wrapper&) = delete;
        GLFW_Window_wrapper(GLFW_Window_wrapper&&) = default;
        ~GLFW_Window_wrapper();

        std::vector<const char*> GetGLFWRequiredInstanceExtensions();

        GLFW_Window_wrapper& operator= (const GLFW_Window_wrapper &) = delete;
        GLFW_Window_wrapper& operator= (GLFW_Window_wrapper&&) = default;

        WindowHandle* GetHandle();

        void PollEvents();
        void WaitEvents();
        bool KeepWindowOpen();
        void SetWindowShouldClose();
        void WaitEventsTimeout(double t);
        void UpdateTitle(const std::string& new_title);

        virtual void WindowLoop();
        virtual void SwapBuffers();
        virtual void ProcessScroll(double x, double y);
        virtual void ProcessTextInput(unsigned int codepoint);
        virtual void WindowResized(int new_width, int new_height);

    };
}

#endif // !GLFW_WRAPPER_HPP

