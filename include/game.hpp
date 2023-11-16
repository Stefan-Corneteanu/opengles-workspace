#pragma once

#include <deque>

namespace opengles_workspace{

    struct Pos{
        float x,y; //normalized position of each body chunk's (+ food's) upper left corner (between -1.0f and 1.0f)
        bool operator==(Pos&);
        bool operator!=(Pos&);
    };

    const float step = 0.2f; //space of snake movements and pieces rendering

    enum Direction{
        UP,DOWN,LEFT,RIGHT,NONE //enum for direction in which snake heads
    };

    float gen_rand_coord(int);

    class Game{

        public:
            Game();
            Game(Pos, Pos);
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

        private:
            Direction snake_dir;
            std::deque<Pos>snake_queue;
            bool snake_has_eaten;
            bool snake_is_alive;
            Pos food_pos;
    };
}