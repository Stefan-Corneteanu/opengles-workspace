#include <common.hpp>

#include <cmath>
#include <algorithm>

namespace opengles_workspace{

    bool Pos::operator==(const Pos& other){
        return this->x == other.x && this->y == other.y;
    }

    bool Pos::operator!=(const Pos& other){
        return this->x != other.x || this->y != other.y;
    }

    Snake::Snake(){}

    Snake::Snake(Pos head, Direction dir){
        this->dir = this->awaiting_next_dir =  dir;
        this->queue = std::deque<Pos>(); 
        queue.push_front(head);
    }

    Snake::~Snake(){}

    const Direction Snake::getDir(){
        return this->dir;
    }

    void Snake::setDir(Direction dir){
        this->dir = dir;
    }

    const Direction Snake::getAwaitingNextDir(){
        return this->awaiting_next_dir;
    }
    
    void Snake::setAwaitingNextDir(Direction dir){
        this->awaiting_next_dir = dir;
    }

    const std::deque<Pos> Snake::getQueue(){
        return this->queue;
    }

    void Snake::pushHead(Pos new_head){
        this->queue.push_front(new_head);
    }

    const bool Snake::isAlive(){
        return this->is_alive;
    }

    void Snake::die(){
        this->is_alive = false;
        this->dir = NONE;
        this->awaiting_next_dir = NONE;
    }

    void Snake::eat(){
        this->has_eaten = true;
    }

    const bool Snake::hasEaten(){
        return this->has_eaten;
    }

    void Snake::makeHungry(){
        this->has_eaten = false;
    }

    /**
     * occupiesPos: checks if a position is already occupied by the snake (is in its queue)
     * @param p: the position to be looked up in the queue
     * @return: true if it is in the snake's queue else false
    */
    const bool Snake::occupiesPos(Pos p){
        return std::find(queue.begin(),queue.end(),p) != queue.end();
    }

    /**
     * move: a function that determine's a snake's new head given its direction of movement and removes the current tail
     * @return: the new head's position
     * @note: given that the game checks for positions being occupied, adding the new head after finding it 
     * would result in occupiesPos to always return true. The head is returned and pushed to the queue after checking for collisions 
     * (SEE Game::move and Game::occupiesPos in game.cpp and Snake::occupiesPos in common.cpp)
    */
    Pos Snake::move(){

        //add new head in queue
        Pos head = this->queue.front();
        Pos new_head; //to be added

        //figure out the coordinates of new head based on the direction and proximity to window border
        switch (this->dir){

        case UP:{
            float new_y;
            if (head.y + STEP > 1.0f){ //teleport to lower bound
                new_y = round((head.y + STEP - 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move up
                new_y = round((head.y + STEP)*100)/100.0f; //roundoff error correction
            }
            new_head = {head.x,new_y};
            break;
            }
            
        case DOWN:{
            float new_y;
            if (head.y - STEP <= -1.0f){ //teleport to upper bound
                new_y = round((head.y - STEP + 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move down
                new_y = round((head.y - STEP)*100)/100.0f; //roundoff error correction
            }
            new_head = {head.x,new_y};
            break;
            }

        case LEFT:{
            float new_x;
            if (head.x - STEP < -1.0f){ //teleport to right bound
                new_x = round((head.x - STEP + 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move left
                new_x = round((head.x - STEP)*100)/100.0f; //roundoff error correction
            }
            new_head = {new_x,head.y};
            break;
            }

        case RIGHT:{
            float new_x;
            if (head.x + STEP >= 1.0f){ //teleport to right bound
                new_x = round((head.x + STEP - 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move left
                new_x = round((head.x + STEP)*100)/100.0f; //roundoff error correction
            }
            new_head = {new_x,head.y};
            break;
            }

        default:{//NONE
            new_head = head;
            break;
            }
        }

        //delete queue if snake hasnt eaten
        if (!this->has_eaten){
            this->queue.pop_back();
        }
        else{
            this->has_eaten = false;
        }

        return new_head;
    }
}