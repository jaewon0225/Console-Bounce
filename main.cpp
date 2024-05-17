#include <iostream>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::milliseconds

#include "solver.hpp"
#include "render.hpp"

int main() {

    Solver solver({20,10}, 1);
    Renderer renderer(80, 20);

    for (int i = 0; i < 2000; i++) {
        for (int i = 0; i < 15; i++) {
            solver.update();
            std::cout << solver.ball.position <<std::endl;
        }
        renderer.renderScene(solver);
        renderer.printScene();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
}
