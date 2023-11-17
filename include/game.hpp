#pragma once

#include <deque>
#include <polled_object.hpp>
#include <renderer.hpp>
#include <common.hpp>

namespace opengles_workspace{

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
    };
}