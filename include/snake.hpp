#pragma once

#include <deque>

namespace opengles_workspace{

    struct Pos{
        float x,y; //normalized position of each body chunk's upper left corner (between -1.0f and 1.0f)
        bool operator==(Pos&);
    };

    const float step = 0.2f; //space of snake movements and pieces rendering

    enum Direction{
        UP,DOWN,LEFT,RIGHT,NONE //enum for direction in which snake heads
    };

    class Snake{

        public:
            Snake(float, float);
            ~Snake();

            Direction getDir();
            void setDir(Direction);

            std::deque<Pos> getQueue();
            bool isAlive();

            void eat();

            bool occupiesPos(Pos);

            void move();

        private:
            Direction dir;
            std::deque<Pos>snake_queue;
            bool has_eaten;
            bool is_alive;
    };
}