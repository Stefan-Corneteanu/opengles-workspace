#pragma once
#include <deque>

//stuff used both by renderer and game logic found here

namespace opengles_workspace{

    struct Pos{
        float x,y; //normalized position of each body chunk's (+ food's) upper left corner (between -1.0f and 1.0f)
        bool operator==(Pos&);
        bool operator!=(Pos&);
    };

    enum Direction{
        UP,DOWN,LEFT,RIGHT,NONE //enum for direction in which snake heads
    };

    const float STEP = 0.2f; //space of snake movements and pieces rendering

    class Snake{

        public:

            Snake();
            Snake(Pos,Direction);
            ~Snake();

            Direction getDir();
            void setDir(Direction);

            Direction getAwaitingNextDir();
            void setAwaitingNextDir(Direction);

            std::deque<Pos> getQueue();
            void removeTail();
            void pushHead(Pos);

            bool isAlive();
            void die();


            void eat();
            bool hasEaten();
            void makeHungry();

            bool occupiesPos(Pos);

            Pos move();

        private:
            Direction dir;
            std::deque<Pos> queue;
            bool has_eaten;
            bool is_alive = true;
            Direction awaiting_next_dir = NONE;
    };
    
}