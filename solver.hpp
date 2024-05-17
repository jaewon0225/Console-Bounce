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
    std::vector<float> velocity = {30.0f, 0.0f};
    int radius;
    
    Ball() = default;

    Ball(std::vector<float> _position, int _radius)
        : position(_position), radius(_radius) 
        {
            //std::cout << "Initialized" << std::endl;
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
        : chunks(1)
        {
            obstacle_map = generateObstacleMap(file_name);
            //std::cout << chunks.size() << std::endl;
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
                //std::cout << line[i] << " ";
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
                    //std::cout << obstacle_map.at(0).size() << std::endl;
                    chunks.at(0).emplace_back(j,20 - i);
                }
            }
        }
        //std::cout << "Filled Chunks" << std::endl;
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
private:
    bool collided = false;

public:
    Ball ball;
    ObstacleMap obstacle_map;
    float dt = 0.005f;
    std::vector<int> window_size = {80, 20};
    std::vector<float> new_p;
    int checked = 0;

    Solver(std::vector<float> ball_position, int ball_radius)
        : ball(ball_position, ball_radius), obstacle_map("../maps/stage_1.txt")
        {}

    void setBounceVelocity(const std::string& dir, const std::vector<float>& corner)
    {   
        if (dir == "up") {
            ball.position.at(0) = new_p.at(0);
            ball.position.at(1) = 2.0f * corner.at(1) - new_p.at(1);
            ball.velocity.at(1) = 30.0f;
            
        }
        else if (dir == "down") {
            ball.position.at(0) = new_p.at(0);
            ball.position.at(1) = 2.0f * corner.at(1) - new_p.at(1);
            ball.velocity.at(1) = -ball.velocity.at(1);
        }
        else if (dir == "left") {
            ball.position.at(0) = 2.0f * corner.at(0) - new_p.at(0);
            ball.position.at(1) = new_p.at(1);
            ball.velocity.at(0) = -ball.velocity.at(0);
            std::cout << "LEFT" << std::endl;
        }
        else if (dir == "right") {
            ball.position.at(0) = 2.0f * corner.at(0) - new_p.at(0);
            ball.position.at(1) = new_p.at(1);
            ball.velocity.at(0) = -ball.velocity.at(0);
            std::cout << "RIGHT" << std::endl;
        }
        new_p = ball.position;     
    }

    void applyGravity() 
    {
        ball.acceleration.at(1) = -50.0f;
    }

    void update()
    {
        applyGravity();
        updateVectors2();
    }

    void updateVectors()
    {
        for (int i = 0; i < ball.position.size(); i++) {
            float position = ball.position.at(i);
            float velocity = ball.velocity.at(i);
            if (position + velocity * dt >= 0 and position + velocity * dt <= window_size.at(i)) {
                ball.position.at(i) += velocity * dt;
            } 
            else {
                if (position + velocity * dt > window_size.at(i)) {
                    ball.position.at(i) = 2 * window_size.at(i) - (position + velocity * dt);
                }
                else{
                    ball.position.at(i) = -velocity * dt - position;
                }
                ball.velocity.at(i) = -velocity;
            }
        }        
    }

    void updateVectors2()
    {
        ball.velocity = ball.velocity + ball.acceleration * dt;
        std::vector<float> x_unit_vel            = ball.velocity / ball.velocity[0];
        std::vector<float> y_unit_vel            = ball.velocity / ball.velocity[1];
        std::vector<std::vector<Coords>> chunks  = obstacle_map.getChunks();
        std::vector<float> curr_p                = ball.position;
        std::vector<float> closest_intersect     = {1000000.0f, 1000000.0f, 1000000.0f}; // {x, y, distance^2 from curr_p}
        std::string direction;
        new_p                                    = ball.position + ball.velocity * dt;
        collided                                 = false;


        for (Coords& coord : chunks[0]) {
            //check if the ball is in the collision zone 
            if (ball.velocity[0] > 0) {
                if (ball.velocity[1] > 0) {
                    projectBall(1, y_unit_vel, {coord.x, coord.y}, {coord.x + 1.1f, coord.y}, curr_p, new_p, closest_intersect, direction, "down");
                    projectBall(2, x_unit_vel, {coord.x, coord.y}, {coord.x, coord.y + 1.1f}, curr_p, new_p, closest_intersect, direction, "left");
                }
                else if (ball.velocity[1] <= 0)
                {
                    projectBall(1, y_unit_vel, {coord.x, coord.y}, {coord.x + 1.1f, coord.y}, curr_p, new_p, closest_intersect, direction, "up");
                    projectBall(2, x_unit_vel, {coord.x + 1.1f, coord.y}, {coord.x + 1.1f, coord.y + 1.1f}, curr_p, new_p, closest_intersect, direction, "left");
                }
                
            }
            else if (ball.velocity[0] <= 0) {
                if (ball.velocity[1] > 0) {
                    projectBall(1, y_unit_vel, {coord.x, coord.y + 1.1f}, {coord.x + 1.1f, coord.y + 1.1f}, curr_p, new_p, closest_intersect, direction, "down");
                    projectBall(2, x_unit_vel, {coord.x, coord.y}, {coord.x, coord.y + 1.1f}, curr_p, new_p, closest_intersect, direction, "right");
                }
                else if (ball.velocity[1] <= 0)
                {
                    projectBall(1, y_unit_vel, {coord.x, coord.y + 1.1f}, {coord.x + 1.1f, coord.y + 1.1f}, curr_p, new_p, closest_intersect, direction, "up");
                    projectBall(2, x_unit_vel, {coord.x + 1.1f, coord.y}, {coord.x + 1.1f, coord.y + 1.1f}, curr_p, new_p, closest_intersect, direction, "right");
                }
            }
        }

        if (collided) {
            setBounceVelocity(direction, {closest_intersect[0], closest_intersect[1]});
        }
        else {
            updateVectors();
        }
    }

    //sides: 1 for horizontal 2 for vertical

    void projectBall(const int& side, const std::vector<float>& unit_vel, const std::vector<float>& coord1, const std::vector<float>& coord2, const std::vector<float>& curr_p, const std::vector<float>& new_p, std::vector<float>& closest_intersect, std::string& dir, const std::string& curr_dir)
    {
        std::vector<float> intersect;
        if (side == 1) 
        {
            intersect = curr_p + unit_vel * (coord1[1] - curr_p[1]);
            if (intersect[0] >= std::min(coord1[0], coord2[0]) && intersect[0] <= std::max(coord1[0], coord2[0])) {
                if ((sgn(curr_p[1] - intersect[1])) != (sgn(new_p[1] - intersect[1]))) {
                    float dist2 = pow(intersect[0] - curr_p[0], 2.0f) + pow(intersect[1] - curr_p[1], 2.0f);
                    if (dist2 < closest_intersect[2]) {
                        closest_intersect = {intersect[0], intersect[1], dist2};
                        dir = curr_dir;
                        collided = true;
                    }
                }
            }
        } 
        else if (side == 2)
        {
            intersect = curr_p + unit_vel * (coord1[0] - curr_p[0]);
            if (intersect[1] >= std::min(coord1[1], coord2[1]) && intersect[1] <= std::max(coord1[1], coord2[1])) {
                if ((sgn(curr_p[0] - intersect[0])) != (sgn(new_p[0] - intersect[0]))) {
                    float dist2 = pow(intersect[0] - curr_p[0], 2.0f) + pow(intersect[1] - curr_p[1], 2.0f);
                    if (dist2 < closest_intersect[2]) {
                        closest_intersect = {intersect[0], intersect[1], dist2};
                        dir = curr_dir;
                        collided = true;
                    }
                }
            }
        }   
    }
};