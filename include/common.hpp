#pragma once
#include <deque>

#include <config.hpp>

//stuff used both by renderer and game logic found here

namespace opengles_workspace{

    struct Pos{
        float x,y; //normalized position of each body chunk's (+ food's) upper left corner (between -1.0f and 1.0f)
        bool operator==(const Pos&);
        bool operator!=(const Pos&);
    };

    enum Direction{
        UP,DOWN,LEFT,RIGHT,NONE //enum for direction in which snake heads
    };

    

    class Snake{

        public:

            Snake();
            Snake(Pos,Direction);
            ~Snake();

            const Direction getDir();
            void setDir(Direction);

            const Direction getAwaitingNextDir();
            void setAwaitingNextDir(Direction);

            const std::deque<Pos> getQueue();

            void pushHead(Pos);

            const bool isAlive();
            void die();


            void eat();
            const bool hasEaten();
            void makeHungry();

            const bool occupiesPos(Pos);

            Pos move();

        private:
            Direction dir;
            std::deque<Pos> queue;
            bool has_eaten;
            bool is_alive = true;
            Direction awaiting_next_dir = NONE;
    };
    
}