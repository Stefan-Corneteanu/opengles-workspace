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
	auto pWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfwCreateWindow(mWidth, mHeight, "Snake", nullptr, nullptr), destroyGlfwWindow);
	if(!pWindow) {
		throw Exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(pWindow.get());
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	MainLoop loop;
	auto ctx = std::make_shared<Context>(pWindow.get());
	std::shared_ptr<Input> pInput(Input::create(ctx));
	std::shared_ptr<GLFWRenderer> pRenderer = std::make_shared<GLFWRenderer>(ctx);

	Snake snake((Pos){genRandCoord(COORD_X), genRandCoord(COORD_Y)}, (Direction)(rand()%4));
	std::shared_ptr<Game> game = std::make_shared<Game>(snake,(Pos){genRandCoord(COORD_X), genRandCoord(COORD_Y)}, pRenderer);

	pInput->registerKeyCallback([&](Key key, KeyMode keyMode) {
			if (key == Key::ESCAPE && keyMode == KeyMode::PRESS) {
				glfwSetWindowShouldClose(pWindow.get(), GLFW_TRUE);
				return false;
			}
			if (key == Key::UP && keyMode == KeyMode::RELEASE && game->isSnakeAlive()){//on key release, only if snake is alive
				//snake should go up
				game->setSnakeAwaitingNextDir(UP);
			}
			if (key == Key::DOWN && keyMode == KeyMode::RELEASE  && game->isSnakeAlive()){//on key release, only if snake is alive
				//snake should await going down
				game->setSnakeAwaitingNextDir(DOWN);
			}
			if (key == Key::LEFT && keyMode == KeyMode::RELEASE  && game->isSnakeAlive()){//on key release, only if snake is alive
				//snake should await going left
				game->setSnakeAwaitingNextDir(LEFT);
			}
			if (key == Key::RIGHT && keyMode == KeyMode::RELEASE  && game->isSnakeAlive()){//on key release, only if snake is alive
				//snake should await going right
				game->setSnakeAwaitingNextDir(RIGHT);
			}
			return true;
		});

	loop.addPolledObject(pInput);
	loop.addPolledObject(pRenderer);
	loop.addPolledObject(game);
	pRenderer->render();
	loop.run();
	return 0;
}
}