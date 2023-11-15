#include "glfw_application.hpp"
#include "exception.hpp"
#include "context.hpp"
#include "input.hpp"
#include "main_loop.hpp"
#include "renderer.hpp"
#include "snake.hpp"

#include <memory>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <ctime>

#define GLFW_WINDOW(ptr) reinterpret_cast<GLFWwindow*>(ptr)

namespace opengles_workspace
{

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

/*
	bool Game::setFoodPos(float x, float y){
        this->food = {x,y};
        if (this->snake.occupiesPos(this->food)){ 
            //we cant overlap food on top of snake, need to find another position. 
            //Solution: linear search on the undeclared matrix of upper left corner positions of the screen
            int size = round(2.0f/step); //undeclared matrix of upper left corner positions of the screen is of size x size
            for (int i = 0; i < size; i++){
                for (int j = 0; j < size; j++){
                    this->food = {-1.0f+i*step,-1.0f+j*step};
                    if (!this->snake.occupiesPos(this->food)){
                        return true;
                    }
                }
            }
        }
        else{
            return true;
        }
        return false; //should never reach here
    }
*/

typedef void(*GLFWwindowDeleter)(GLFWwindow*);

void error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW error: %s\n", description);
}

void window_callback(GLFWwindow* window, int new_width, int new_heigth){
	glViewport(0,0,new_width,new_heigth);
	glfwSwapBuffers(window);
	printf("%d %d\n", new_width, new_heigth);
}

void destroyGlfwWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
}

GlfwApplication::GlfwApplication(size_t width, size_t height)
	: mWidth(width)
	, mHeight(height)
{
	if(!glfwInit()) {
        throw Exception("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(error_callback);

	srand(time(0)); //used for generating random position of snake
}

GlfwApplication::~GlfwApplication() {
	glfwTerminate();
}

int GlfwApplication::run() {
	auto pWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfwCreateWindow(mWidth, mHeight, "OpenGL ES workspace", nullptr, nullptr), destroyGlfwWindow);
	if(!pWindow) {
		throw Exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(pWindow.get());
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	Snake s(gen_rand_coord(0),gen_rand_coord(1)); //Set snake at random positions
	s.setDir((Direction)(rand()%4)); //give snake random direction
	///everything related to food position should be done both at initialization and everytime the snake's head
	///moves to the food position

	MainLoop loop;
	auto ctx = std::make_shared<Context>(pWindow.get());
	std::shared_ptr<Input> pInput(Input::create(ctx));
	std::shared_ptr<GLFWRenderer> pRenderer = std::make_shared<GLFWRenderer>(ctx);
	pInput->registerKeyCallback([&](Key key, KeyMode keyMode) {
			if (key == Key::ESCAPE && keyMode == KeyMode::PRESS) {
				glfwSetWindowShouldClose(pWindow.get(), GLFW_TRUE);
				return false;
			}
			if (key == Key::UP && keyMode == KeyMode::RELEASE && s.isAlive() && //on key release, only if snake is alive
				(s.getQueue().size() == 1 || s.getDir() != DOWN)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go up
				s.setDir(UP);
			}
			if (key == Key::DOWN && keyMode == KeyMode::RELEASE  && s.isAlive() && //on key release, only if snake is alive
				(s.getQueue().size() == 1 || s.getDir() != UP)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go down
				s.setDir(DOWN);
			}
			if (key == Key::LEFT && keyMode == KeyMode::RELEASE  && s.isAlive() && //on key release, only if snake is alive
				(s.getQueue().size() == 1 || s.getDir() != RIGHT)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go left
				s.setDir(LEFT);
			}
			if (key == Key::RIGHT && keyMode == KeyMode::RELEASE  && s.isAlive() && //on key release, only if snake is alive
				(s.getQueue().size() == 1 || s.getDir() != LEFT)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go right
				s.setDir(RIGHT);
			}
			return true;
		});

	loop.addPolledObject(pInput);
	loop.addPolledObject(pRenderer);
	pRenderer->render();

	/* didnt work, couldnt convert the lambda to an acceptable function
	glfwSetFramebufferSizeCallback(pWindow.get(),(GLFWframebuffersizefun)[&](GLFWwindow* window, int new_width, int new_heigth){
		glViewport(0,0,new_width,new_heigth);
		glfwSwapBuffers(window);
		pRenderer->render();
		printf("%d %d\n", new_width, new_heigth);
	});
	*/

	loop.run();
	return 0;
}
}