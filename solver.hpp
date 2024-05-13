#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>

#include "math.hpp"

class Ball
{
public:
    std::vector<float> position = {0.0f, 0.0f};
    float max_velocity = 3.0f;
    std::vector<float> acceleration = {0.0f, -100.0f};
    std::vector<float> velocity = {30.0f, 30.0f};
    int radius;
    
    Ball() = default;

    Ball(std::vector<float> _position, int _radius)
        : position(_position), radius(_radius) 
        {
            std::cout << "Initialized" << std::endl;
        }
};

class ObstacleMap
{
private:
    std::vector<std::vector<char>> obstacle_map;
    Coords starting_point;
    std::vector<std::vector<Coords>> chunks;

public:
    ObstacleMap() = default;

    ObstacleMap(std::string file_name)
        : chunks(4)
        {
            obstacle_map = generateObstacleMap(file_name);
            std::cout << chunks.size() << std::endl;
            fillChunks();
        }

    std::vector<std::vector<char>> generateObstacleMap(const std::string& filename) {
        std::vector<std::vector<char>> vectorOfVectors;

        // Open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return vectorOfVectors; // Return empty vector if file cannot be opened
        }

        // Read file line by line
        std::string line;
        int line_num = 0;
        while (std::getline(file, line)) {
            std::vector<char> vec;
            // Iterate through each character in the line
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == 'X') {
                    line[i] = ' ';
                    starting_point.x = i;
                    starting_point.y = line_num;
                }
                std::cout << line[i] << " ";
                vec.push_back(line[i]); // Add character to vector
            }
            vectorOfVectors.push_back(vec); // Add vector to vector of vectors
        }

        file.close(); // Close the file
        return vectorOfVectors;
    }

    std::vector<std::vector<char>>& getMap()
    {
        return obstacle_map;
    }

    void fillChunks()
    {
        for (int i = 0; i < obstacle_map.size(); i++) {
            for (int j = 0; j < obstacle_map.at(0).size(); j++) {
                if (obstacle_map[i][j] == 'M') {
                    std::cout << obstacle_map.at(0).size() << std::endl;
                    chunks.at(j/20).emplace_back(j,20 - i);
                }
            }
        }
        std::cout << "Filled Chunks" << std::endl;
    }

    std::vector<std::vector<Coords>>& getChunks()
    {
        return chunks;
    }

    void loadStage(std::string map_path) 
    {
        obstacle_map = generateObstacleMap(map_path);
    }
};

class Solver
{
public:
    Ball ball;
    ObstacleMap obstacle_map;
    float dt = 0.01f;
    std::vector<int> window_size = {80, 40};
    std::vector<float> new_p;
    int checked = 0;

    Solver(std::vector<float> ball_position, int ball_radius)
        : ball(ball_position, ball_radius), obstacle_map("../maps/stage_1.txt")
        {}

    void checkCollision(const Coords& obstacle_coords) 
    {   
        if (std::abs(new_p.at(0) - obstacle_coords.x) < 1.0f && std::abs(new_p.at(1) - obstacle_coords.y) < 1.0f) {
            calculateBounce(obstacle_coords);   
        }
        else {
            ball.position = new_p;
        }
    }

    void calculateBounce(const Coords& obstacle_coords)
    {   
        std::vector<float> corner;
        std::vector<float> projected_diff;
        
        if (ball.position.at(0) < obstacle_coords.x) {
            if (ball.position.at(1) < obstacle_coords.y) {
                corner = {obstacle_coords.x - 0.5f, obstacle_coords.y - 0.5f};
                projected_diff = ball.position - (corner + ball.velocity * ((obstacle_coords.x - corner.at(0)) / ball.velocity.at(0)));
                if (projected_diff.at(1) > 0) {
                    setBounceVelocity("left", corner);
                }
                else {
                    setBounceVelocity("down", corner);
                }
            } 
            else {
                corner = {obstacle_coords.x - 0.5f, obstacle_coords.y + 0.5f};
                projected_diff = ball.position - (corner + ball.velocity * ((obstacle_coords.x - corner.at(0)) / ball.velocity.at(0)));
                if (projected_diff.at(1) > 0) {
                    setBounceVelocity("up", corner);
                }
                else {
                    setBounceVelocity("left", corner);
                }
            }
        } 
        else {
            if (ball.position.at(1) < obstacle_coords.y) {
                corner = {obstacle_coords.x + 0.5f, obstacle_coords.y - 0.5f};
                projected_diff = ball.position - (corner + ball.velocity * ((obstacle_coords.x - corner.at(0)) / ball.velocity.at(0)));
                if (projected_diff.at(1) > 0) {
                    setBounceVelocity("right", corner);
                }
                else {
                    setBounceVelocity("down", corner);
                }
            } 
            else {
                corner = {obstacle_coords.x + 1.5f, obstacle_coords.y + 1.5f};
                projected_diff = ball.position - (corner + ball.velocity * ((obstacle_coords.x - corner.at(0)) / ball.velocity.at(0)));
                if (projected_diff.at(1) > 0) {
                    setBounceVelocity("up", corner);
                }
                else {
                    setBounceVelocity("right", corner);
                }
            }
        }
    }

    void setBounceVelocity(const std::string& dir, const std::vector<float>& corner)
    {   
        if (dir == "up") {
            ball.position.at(0) = new_p.at(0);
            ball.position.at(1) = 2.0f * corner.at(1) - new_p.at(1) - 0.5f;
            ball.velocity.at(1) = 100.0;
            
        }
        else if (dir == "down") {
            ball.position.at(0) = new_p.at(0);
            ball.position.at(1) = 2.0f * corner.at(1) - new_p.at(1) + 0.5f;
            ball.velocity.at(1) = -1.0f * ball.velocity.at(1);
        }
        else if (dir == "left") {
            ball.position.at(0) = 2.0f * corner.at(0) - new_p.at(0) - 0.5f;
            ball.position.at(1) = new_p.at(1);
            ball.velocity.at(0) = -ball.velocity.at(0);
        }
        else if (dir == "right") {
            ball.position.at(0) = 2.0f * corner.at(0) - new_p.at(0) + 0.5f;
            ball.position.at(1) = new_p.at(1);
            ball.velocity.at(0) = -ball.velocity.at(0);
        }
        new_p = ball.position;
        // std::cout << "done!" << std::endl;
        
    }

    void applyGravity() 
    {
        ball.acceleration.at(1) = -500.0f;
    }

    void update()
    {   
        applyGravity();
        // updateVectors();
        ball.velocity = ball.velocity + ball.acceleration * dt;
        new_p = ball.position + ball.velocity * dt;
        std::vector<Coords> chunk = obstacle_map.getChunks()[floor(ball.position.at(0)/20)];
        // std::cout << chunk << std::endl;
        for (Coords& coord : chunk) {
            checkCollision(coord);
        }
        // std::cout << checked << std::endl;
        std::cout << ball.position.at(0) << " " << ball.position.at(1) << std::endl;
    }
};