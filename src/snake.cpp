#include <snake.hpp>
#include <deque>
#include <cmath>

namespace opengles_workspace{

    bool Pos::operator==(Pos& other){
        return this->x == other.x && this->y == other.y;
    }

    Snake::Snake(float x, float y){
        this->is_alive = true;
        this->snake_queue = std::deque<Pos>();
        this->snake_queue.push_front((Pos){x,y});
    }

    Snake::~Snake(){}

    Direction Snake::getDir(){
        return this->dir;
    }

    void Snake::setDir(Direction dir){
        this->dir = dir;
    }

    std::deque<Pos> Snake::getQueue(){
        return this->snake_queue;
    }

    bool Snake::isAlive(){
        return this->is_alive;
    }

    void Snake::eat(){
        this->has_eaten = true;
    }

    bool Snake::occupiesPos(Pos p){
        //the need to iterate through the queue is the reason why it is a deque
        for (Pos piece: this->snake_queue){
            if (piece == p){
                return true;
            }
        }
        return false;
    }

    void Snake::move(){

        //add new head in queue
        Pos head = this->snake_queue.front();
        Pos new_head; //to be added

        //figure out the coordinates of new head based on the direction and proximity to window border
        switch (this->dir){

        case UP:{
            float new_y;
            if (head.y + step > 1.0f){ //teleport to lower bound
                new_y = round((head.y + step - 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move up
                new_y = round((head.y + step)*100)/100.0f; //roundoff error correction
            }
            new_head = {head.x,new_y};
            break;
            }
            
        case DOWN:{
            float new_y;
            if (head.y - step <= -1.0f){ //teleport to upper bound
                new_y = round((head.y - step + 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move down
                new_y = round((head.y - step)*100)/100.0f; //roundoff error correction
            }
            new_head = {head.x,new_y};
            break;
            }

        case LEFT:{
            float new_x;
            if (head.x - step < -1.0f){ //teleport to right bound
                new_x = round((head.x - step + 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move left
                new_x = round((head.x - step)*100)/100.0f; //roundoff error correction
            }
            new_head = {new_x,head.y};
            break;
            }

        case RIGHT:{
            float new_x;
            if (head.x + step >= 1.0f){ //teleport to right bound
                new_x = round((head.x + step - 2.0f)*100)/100.0f; //roundoff error correction
            }
            else{ //just move left
                new_x = round((head.x + step)*100)/100.0f; //roundoff error correction
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
            this->snake_queue.pop_back();
        }
        else{
            this->has_eaten = false;
        }

        //check for collision with
        //1 snake body
        if(this->occupiesPos(new_head)){
            this->is_alive = false;
            this->dir = NONE;
            return;
        }

        //2 food
        //to be redone

        //if we got till here, add new head in tail
        this->snake_queue.push_front(new_head);
    }
}