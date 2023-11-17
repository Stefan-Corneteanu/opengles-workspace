#pragma once
#include <memory>
#include <optional>
#include <deque>
#include <context.hpp>
#include <polled_object.hpp>
#include <common.hpp>


#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace opengles_workspace
{
class GLFWRenderer : public PolledObject
	{
	public:
		GLFWRenderer(std::shared_ptr<Context> context);

		~GLFWRenderer() = default;

		void render(std::deque<Pos>,Pos);

		bool poll() override;
	private:

		std::shared_ptr<Context> mContext;
		GLuint program;
		unsigned int VAO,VBO,IBO;

		GLFWwindow* window() const { return static_cast<GLFWwindow*>(mContext->window()); }
	};
}