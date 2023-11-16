#include "glfw_application.hpp"
#include "exception.hpp"
#include "context.hpp"
#include "input.hpp"
#include "main_loop.hpp"
#include "renderer.hpp"
#include "game.hpp"

#include <memory>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <ctime>

#define GLFW_WINDOW(ptr) reinterpret_cast<GLFWwindow*>(ptr)

namespace opengles_workspace
{
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

	Game game({gen_rand_coord(0),gen_rand_coord(1)},{gen_rand_coord(0),gen_rand_coord(1)}); //Set snake and food at random positions
	game.setSnakeDir((Direction)(rand()%4)); //give snake random direction

	MainLoop loop;
	auto ctx = std::make_shared<Context>(pWindow.get());
	std::shared_ptr<Input> pInput(Input::create(ctx));
	std::shared_ptr<GLFWRenderer> pRenderer = std::make_shared<GLFWRenderer>(ctx);
	pInput->registerKeyCallback([&](Key key, KeyMode keyMode) {
			if (key == Key::ESCAPE && keyMode == KeyMode::PRESS) {
				glfwSetWindowShouldClose(pWindow.get(), GLFW_TRUE);
				return false;
			}
			if (key == Key::UP && keyMode == KeyMode::RELEASE && game.isSnakeAlive() && //on key release, only if snake is alive
				(game.getSnakeQueue().size() == 1 || game.getSnakeDir() != DOWN)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go up
				game.setSnakeDir(UP);
			}
			if (key == Key::DOWN && keyMode == KeyMode::RELEASE  && game.isSnakeAlive() && //on key release, only if snake is alive
				(game.getSnakeQueue().size() == 1 || game.getSnakeDir() != UP)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go down
				game.setSnakeDir(DOWN);
			}
			if (key == Key::LEFT && keyMode == KeyMode::RELEASE  && game.isSnakeAlive() && //on key release, only if snake is alive
				(game.getSnakeQueue().size() == 1 || game.getSnakeDir() != RIGHT)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go left
				game.setSnakeDir(LEFT);
			}
			if (key == Key::RIGHT && keyMode == KeyMode::RELEASE  && game.isSnakeAlive() && //on key release, only if snake is alive
				(game.getSnakeQueue().size() == 1 || game.getSnakeDir() != LEFT)){ //and either snake has only one chunk or its not goin the opposite way
				//snake should go right
				game.setSnakeDir(RIGHT);
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