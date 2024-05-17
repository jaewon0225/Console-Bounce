#pragma once
#include <iostream>
#include <cmath>
#include <deque>
#include <vector>

#include "solver.hpp"

class Renderer
{
private:
    const int background_width;
    const int background_height;
    std::vector<std::vector<char>> background;



public:
    
    Renderer(int width, int height)
        : background_width(width), background_height(height) 
        {
            background.resize(background_height, std::vector<char>(background_width, ' '));
        }

    void renderScene(Solver& solver)
    {   
        // setBackground(' ');
        background = solver.obstacle_map.getMap();
        insertBall(solver);
    }

    void insertBall(Solver& solver)
    {
        int ball_x_pos = std::floor(solver.ball.position.at(0));
        int ball_y_pos = std::floor(background_height - solver.ball.position.at(1));
        background[ball_y_pos][ball_x_pos] = 'o';
    }

    void setBackground(int num)
    {
        for (int i = 0; i < background_height; i++) {
            for (int j = 0; j < background_width; j++) {
                background[i][j] = num; // Set each element to 1
            }
        }
    }
    
    void printScene()
    {   

        #ifdef _WIN32
            system("cls"); // Clear console on Windows
        #else
            system("clear"); // Clear console on other systems (Linux, macOS)
        #endif
        
        std::string mString(82, 'M');
        std::cout << mString << std::endl;
        for (auto& vec : background) {
            std::cout << "M";
            for (auto& pix : vec) {
                std::cout << pix;
            }
            std::cout << "M" << std::endl;
        }
        std::cout << mString << std::endl;
    }





    
};