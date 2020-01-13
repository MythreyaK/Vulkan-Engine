
#pragma once
#ifndef GAME_MAIN_HPP
#define GAME_MAIN_HPP

#include <string>
//#include <glm/glm.hpp>
#include "version.hpp"
#include "Window/GLFW.hpp"
#include "Renderer.hpp"

class GameWindow : public Engine::Window::GLFW_Window_wrapper {

private:
    Engine::Render::Renderer renderer;

public:
    GameWindow(int w, int h, const std::string& title);
    void WindowLoop() override;
};

#endif // !GAME_MAIN_H
