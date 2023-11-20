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

            Direction getSnakeDir();
            void setSnakeDir(Direction);

            Direction getSnakeAwaitingNextDir();
            void setSnakeAwaitingNextDir(Direction);

            Pos getFoodPos();
            bool setFoodPos(float, float);

            std::deque<Pos> getSnakeQueue();
            bool isSnakeAlive();

            void snakeEat();

            bool snakeOccupiesPos(Pos);

            void snakeMove();

            bool poll() override;

        private:
            Snake snake;
            Pos food_pos;
            std::shared_ptr<GLFWRenderer> renderer = nullptr;

            const int no_millis = 1000;
            const std::chrono::milliseconds frame_dur = std::chrono::milliseconds(1) * no_millis;
            std::chrono::high_resolution_clock::time_point frame_start = std::chrono::high_resolution_clock::now();
    };
}