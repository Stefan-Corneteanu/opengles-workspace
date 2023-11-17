#include <game.hpp>
#include <deque>
#include <cmath>
#include <stdio.h>

namespace opengles_workspace{

    float gen_rand_coord(int coord){ //coord refers to if we talk about x or y coordinate

        float val = rand() % 5 * 0.2f; //value between 0 and 0.8f

        if (coord == 0){ //x coordinate should be between -1.0f and 0.8f;
            if (rand()%2){ //to make the coordinate on either half of the window
                val *= -1;
                val -= 0.2f;
            }
        }
        else{ //y coordinate should be between -0.8f and 1.0f
            if (rand()%2){
                val *= -1;
            }
            else{
                val += 0.2f;
            }
        }
        
        return val;
    }

    Game::Game(Pos snake_head_pos, Direction snake_dir, Pos food_pos, std::shared_ptr<GLFWRenderer> renderer){

        this->snake_queue = std::deque<Pos>();
        this->snake_queue.push_front(snake_head_pos);
        this->snake_dir = snake_dir; //give snake random direction
        if (food_pos!=snake_head_pos){
            this->food_pos = food_pos;
        }
        else{
            this->setFoodPos(gen_rand_coord(0),gen_rand_coord(1));
        }

        this->renderer = renderer;
    }

    Game::~Game(){}

    Direction Game::getSnakeDir(){
        return this->snake_dir;
    }

    void Game::setSnakeDir(Direction dir){
        this->snake_dir = dir;
    }

    Pos Game::getFoodPos(){
        return this->food_pos;
    }

    bool Game::setFoodPos(float x, float y){
        this->food_pos = {x,y};
        if (this->snakeOccupiesPos(this->food_pos)){ 
            //we cant overlap food on top of snake, need to find another position. 
            //Solution: linear search on the undeclared matrix of upper left corner positions of the screen
            int size = round(2.0f/step); //undeclared matrix of upper left corner positions of the screen is of size x size
            for (int i = 0; i < size; i++){
                for (int j = 0; j < size; j++){
                    this->food_pos = {-1.0f+i*step,-1.0f+j*step};
                    if (!this->snakeOccupiesPos(this->food_pos)){
                        return true; //found position
                    }
                }
            }
        }
        else{
            return true; //original position was good
        }
        return false; //should never reach here
    }

    std::deque<Pos> Game::getSnakeQueue(){
        return this->snake_queue;
    }

    bool Game::isSnakeAlive(){
        return this->snake_is_alive;
    }

    void Game::snakeEat(){
        this->snake_has_eaten = true;
        //reposition the food;
        this->setFoodPos(gen_rand_coord(0),gen_rand_coord(1));
    }

    bool Game::snakeOccupiesPos(Pos p){
        //the need to iterate through the queue is the reason why it is a deque
        for (Pos piece: this->snake_queue){
            if (piece == p){
                return true;
            }
        }
        return false;
    }

    void Game::snakeMove(){

        //add new head in queue
        Pos head = this->snake_queue.front();
        Pos new_head; //to be added

        //figure out the coordinates of new head based on the direction and proximity to window border
        switch (this->snake_dir){

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
        if (!this->snake_has_eaten){
            this->snake_queue.pop_back();
        }
        else{
            this->snake_has_eaten = false;
        }

        //check for collision with
        //1 snake body
        if(this->snakeOccupiesPos(new_head)){
            this->snake_is_alive = false;
            this->snake_dir = NONE;
            return;
        }

        //2 food
        if (new_head == food_pos){
            this->snakeEat();
        }

        //if we got till here, add new head in tail
        this->snake_queue.push_front(new_head);
    }
    

    bool Game::poll(){

        //TO DO: MOVE ALL THE TIMING RELATED STUFF TO RENDERER
        std::chrono::high_resolution_clock::time_point crt = std::chrono::high_resolution_clock::now(); //current point in time

        if (crt - frame_start >= frame_dur){ //current time minus frame start greater than frame duration? make a move
            this->renderer->render(snake_queue,food_pos);
            frame_start = crt;
            this->snakeMove();
        }
        
        return this->snake_is_alive;
    }
}