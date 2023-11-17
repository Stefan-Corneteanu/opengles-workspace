#pragma once

#include <deque>
#include <chrono>

#include <polled_object.hpp>
#include <renderer.hpp>
#include <common.hpp>

namespace opengles_workspace{

    enum Direction{
        UP,DOWN,LEFT,RIGHT,NONE //enum for direction in which snake heads
    };

    float gen_rand_coord(int);

    class Game: public PolledObject{

        public:
            Game(Pos, Direction, Pos, std::shared_ptr<GLFWRenderer>);
            ~Game();

            Direction getSnakeDir();
            void setSnakeDir(Direction);

            Pos getFoodPos();
            bool setFoodPos(float, float);

            std::deque<Pos> getSnakeQueue();
            bool isSnakeAlive();

            void snakeEat();

            bool snakeOccupiesPos(Pos);

            void snakeMove();

            bool poll() override;

        private:
            Direction snake_dir;
            std::deque<Pos>snake_queue;
            bool snake_has_eaten;
            bool snake_is_alive = true;
            Pos food_pos;
            std::shared_ptr<GLFWRenderer> renderer = nullptr;

            //for controlling time in game (TO DO, MOVE THIS TO RENDERER)
            const int fps = 30;
            const std::chrono::milliseconds no_millis_in_second = std::chrono::milliseconds(1000);
            const std::chrono::milliseconds frame_dur = no_millis_in_second / fps;
            std::chrono::high_resolution_clock::time_point frame_start = std::chrono::high_resolution_clock::now();
    };
}