#ifndef GAME_MAIN_HPP
#define GAME_MAIN_HPP

#include "Window/GLFW.hpp"

#include <string>

namespace Engine::Render {
    class Renderer;
}

class GameWindow : public Engine::Window::GLFW_Window_wrapper {

private:
    std::unique_ptr<Engine::Render::Renderer> renderer;

public:
    GameWindow(int w, int h, const std::string& title);
    void WindowLoop() override;
    static void DumpVersion();
};

#endif // !GAME_MAIN_H
