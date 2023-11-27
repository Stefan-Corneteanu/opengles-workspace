#pragma once
#include <memory>
#include <optional>
#include <deque>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <context.hpp>
#include <polled_object.hpp>
#include <common.hpp>

namespace opengles_workspace
{
class GLFWRenderer : public PolledObject
	{
	public:
		GLFWRenderer(std::shared_ptr<Context> context);

		~GLFWRenderer() = default;

		std::string readShaderFile(const char*);

		GLuint loadShader(const char*, GLenum);
		void setupProgram(GLuint, GLuint);
		unsigned int setupTexture(const char*);

		void draw(unsigned int);

		void setData(Snake, Pos);

		void render();

		bool poll() override;
	private:

		std::shared_ptr<Context> mContext;
		GLuint program;
		unsigned int VAO,VBO,IBO;

		unsigned int food_texture, snake_body_texture;

		Snake snake;
		Pos food_pos;

		//constants
		const int STRIDE = 5;
		const int OFFSET_TEX = 3;
		const int NO_VERTICES = 4;
		const int NO_VALS_PER_VERTEX = 5;
		const int NO_POS_VALS_PER_VERTEX = 3;
		const int NO_TEX_VALS_PER_VERTEX = 2;
		const int NO_PRIMITIVES = 2;
		const int NO_INDICES_PER_PRIMITIVE = 3;

		GLFWwindow* window() const { return static_cast<GLFWwindow*>(mContext->window()); }
	};
}