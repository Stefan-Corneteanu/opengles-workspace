#pragma once

#include <deque>
#include <chrono>

#include <polled_object.hpp>
#include <renderer.hpp>
#include <common.hpp>

namespace opengles_workspace{

    enum Coord{
        COORD_X, COORD_Y
    };

    float genRandCoord(Coord);

    class Game: public PolledObject{

        public:

            Game(Snake, Pos, std::shared_ptr<GLFWRenderer>);
            ~Game();

            const Direction getSnakeDir();
            void setSnakeDir(Direction);

            const Direction getSnakeAwaitingNextDir();
            void setSnakeAwaitingNextDir(Direction);

            const Pos getFoodPos();
            bool setFoodPos(float, float);

            const std::deque<Pos> getSnakeQueue();
            const bool isSnakeAlive();

            void snakeEat();

            bool snakeOccupiesPos(Pos);

            void snakeMove();

            bool poll() override;

        private:
            Snake snake;
            Pos food_pos;
            std::shared_ptr<GLFWRenderer> renderer = nullptr;

            
            const std::chrono::milliseconds FRAME_DUR = std::chrono::milliseconds(NO_MILLIS_PER_FRAME);
            std::chrono::high_resolution_clock::time_point frame_start = std::chrono::high_resolution_clock::now();
    };
}