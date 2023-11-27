#include <cmath>
#include <deque>

#include <game.hpp>
#include <config.hpp>

namespace opengles_workspace{

    /**
     * genRandCoord: function to generate a random coordinate (X or Y) in the acceptable interval
     * @param coord: a Coordinate type (X or Y)
     * for X coordinate, the interval of values is [-1.0f, 1.0f - STEP]
     * for Y coordinate, the interval of values is [-1.0f + STEP, 1.0f]
     * both values are multiples of STEP
     * @return: the randomly generated coordinate value for X or for Y
    */
    float genRandCoord(Coord coord){ //coord refers to if we talk about x or y coordinate

        float val = rand() % (int)(1.0f/STEP) * STEP; //value between 0 and 1.0f - step

        if (coord == COORD_X){ //x coordinate should be between -1.0f and 1.0f - step;
            if (rand()%2){ //to make the coordinate on either half of the window
                val *= -1;
                val -= STEP;
            }

        }
        else{ //y coordinate should be between -1.0f + step and 1.0f
            if (rand()%2){
                val *= -1;
            }
            else{
                val += STEP;
            }
        }
        return val;
    }

    Game::Game(Snake snake, Pos food_pos, std::shared_ptr<GLFWRenderer> renderer){
        
        this->snake = snake; //initialize snake
        if (food_pos!=snake.getQueue().front()){ //check that food and snake dont overlap at instantiation
            this->food_pos = food_pos;
        }
        else{
            this->setFoodPos(genRandCoord(COORD_X),genRandCoord(COORD_Y));
        }

        this->renderer = renderer;
        renderer->setData(snake,food_pos);
    }

    Game::~Game(){}

    const Direction Game::getSnakeDir(){
        return this->snake.getDir();
    }

    void Game::setSnakeDir(Direction dir){
        this->snake.setDir(dir);
    }

    const Direction Game::getSnakeAwaitingNextDir(){
        return this->snake.getAwaitingNextDir();
    }

    void Game::setSnakeAwaitingNextDir(Direction dir){
        this->snake.setAwaitingNextDir(dir);
    }

    const Pos Game::getFoodPos(){
        return this->food_pos;
    }

    /**
     * setFoodPos: a more complex approach on setting the position of the food in order to avoid setting it on a
     * position occupied by the snake
     * @param x: x coordinate of food position
     * @param y: y coordinate of food position
     * @return: true if position was found, false otherwise 
     * (returning false is an unexpected behaviour and should be investigated)
    */
    bool Game::setFoodPos(float x, float y){
        this->food_pos = {x,y};
        if (this->snakeOccupiesPos(this->food_pos)){ 
            //we cant overlap food on top of snake, need to find another position. 
            //Solution: linear search on the undeclared matrix of upper left corner positions of the screen
            for (float i = -1.0f; i < 1.0; i = round((i + STEP)*100)/100.0f){
                for (float j = -1.0 + STEP; j <= 1.0; j = round((j + STEP)*100)/100.0f){
                    this->food_pos = {i,j};
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

    const std::deque<Pos> Game::getSnakeQueue(){
        return this->snake.getQueue();
    }

    const bool Game::isSnakeAlive(){
        return this->snake.isAlive();
    }

    /**
     * snakeEat: a function called whenever the snake collides with the food
     * the snake's has_eaten field is set to true (SEE Snake::eat IN common.cpp)
     * and the food's position is reset (SEE setFoodPos)
    */
    void Game::snakeEat(){
        this->snake.eat();
        //reposition the food;
        this->setFoodPos(genRandCoord(COORD_X),genRandCoord(COORD_Y));
    }

    /**
     * snakeOccupiesPos: a function that checks if the snake occupies a particular position (SEE ALSO snake::occupiesPos in common.cpp)
     * @param p: the position that we want to check if is occupied by snake or not (if it is in its queue)
     * @return: true if p is in the snake's queue else false
    */
    bool Game::snakeOccupiesPos(Pos p){
        return this->snake.occupiesPos(p);
    }

    /**
     * snakeMove: a function that determines in the background the movement of the snake
     * a new head is determined and the tail is popped (SEE ALSO snake::move in common.cpp)
     * then it checks for occupying the position of the food or of a body piece
    */
    void Game::snakeMove(){

        Pos new_head = this->snake.move();

        //check for collision with
        //1 snake body
        if(this->snakeOccupiesPos(new_head)){
            this->snake.die();
            return;
        }

        //if we got till here, add new head in tail
        this->snake.pushHead(new_head); //first add the new head then check for collision with food
        //otherwise the food may be repositioned in the same place where it was!

        //2 food
        if (new_head == food_pos){
            this->snakeEat();
        }

    }
    
/**
 * poll: a function that checks if a movement should occur given the time elapsed from last frame.
 * It checks if an inputted user movement is valid or not, calls the snake movement, sends the data
 * to the renderer and asks for it to draw
 * @return: true if the game should still poll else false
*/
    bool Game::poll(){

        std::chrono::high_resolution_clock::time_point crt = std::chrono::high_resolution_clock::now();
			if (crt-frame_start>=FRAME_DUR && (this->isSnakeAlive() || this->getSnakeQueue().size() > 1)){

                //set the inputted snake direction iff it is not opposing of the current direction or the snake is of size 1
                if(!(this->getSnakeDir() == UP && this->getSnakeAwaitingNextDir() == DOWN ||
                    this->getSnakeDir() == DOWN && this->getSnakeAwaitingNextDir() == UP ||
                    this->getSnakeDir() == LEFT && this->getSnakeAwaitingNextDir() == RIGHT ||
                    this->getSnakeDir() == RIGHT && this->getSnakeAwaitingNextDir() == LEFT) ||
                    this->getSnakeQueue().size() == 1){

                    this->setSnakeDir(this->getSnakeAwaitingNextDir());
                }
                this->snakeMove();
                this->renderer->setData(snake,food_pos);
				this->renderer->render();
				frame_start = crt;
			}
        bool renderer_poll = this->renderer->poll();    
        return renderer_poll;
    }
}