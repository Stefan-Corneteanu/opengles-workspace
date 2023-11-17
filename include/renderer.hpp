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

		std::string ReadShaderFile(const char*);

		GLuint LoadShader(const char*, GLenum);
		void setupProgram(GLuint, GLuint);
		void setupTexture(unsigned int&, const char*);

		void draw(unsigned int);

		void render(std::deque<Pos>,Pos);

		bool poll() override;
	private:

		std::shared_ptr<Context> mContext;
		GLuint program;
		unsigned int VAO,VBO,IBO;

		unsigned int food_texture, snake_body_texture;

		//constants
		const int stride = 5;
		const int offsetTex = 3;
		const int noVertices = 4;
		const int noValsPerVertex = 5;
		const int noPosValsPerVertex = 3;
		const int noTexValsPerVertex = 2;
		const int noPrimitives = 2;
		const int noIndicesPerPrimitive = 3;

		GLFWwindow* window() const { return static_cast<GLFWwindow*>(mContext->window()); }
	};
}