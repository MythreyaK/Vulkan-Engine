#include "game.hpp"
#include "Render/Instance/Instance.hpp"
#include "Render/Surface/Surface.hpp"
#include <chrono>

using namespace Engine;

int main(void) {

    try {
        GameWindow termWindow{ GameWindow(800, 600, std::string("Yay!")) };

        termWindow.WindowLoop();
    }
    catch (std::exception e){
        std::cerr << "Exception " << e.what() << "was raised.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

GameWindow::GameWindow(int w, int h, const std::string& title) :
    GLFW_Window_wrapper(w, h, title), renderer(GetGLFWRequiredInstanceExtensions(), GetHandle()) {
}

void GameWindow::WindowLoop() {

    auto start = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::high_resolution_clock::now() - start;
    auto tdiff = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    int count = 0;
    while (KeepWindowOpen()) {

        PollEvents();
        // draw frame
        renderer.DrawFrame();
        ++count;

        diff = std::chrono::high_resolution_clock::now() - start;
        tdiff = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

        if (tdiff.count() >= 1000) {
            start = std::chrono::high_resolution_clock::now();
            std::cerr << "\rFPS: " << count << "      ";
            count = 0;
        }

    }

    renderer.WaitDevice();
}

