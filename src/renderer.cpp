#include <renderer.hpp>
#include <exception.hpp>

#include <memory>
#include <optional>
#include <cassert>
#include <array>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace opengles_workspace
{
	GLFWRenderer::GLFWRenderer(std::shared_ptr<Context> context)
		: mContext(std::move(context))
	{
		//shader codes

		//vertex shader
		const char* vShaderStr =
		"#version 300 es \n"
		"layout(location = 0) in vec4 vPosition; \n"
		"void main() \n"
		"{ \n"
		" gl_Position = vPosition; \n"
		"} \n";

		//fragment shader
		const char* fShaderStr =
		"#version 300 es \n"
		"precision mediump float; \n"
		"uniform vec4 myColor; \n"
		"out vec4 fragColor; \n"
		"void main() \n"
		"{ \n"
		" fragColor = myColor; \n"
		"} \n";

		//compile shaders

		//vertex shader
		const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader,1,&vShaderStr,NULL);
		glCompileShader(vertex_shader);


		//fragment shader
		const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader,1,&fShaderStr,NULL);
		glCompileShader(fragment_shader);

		//link shaders to program
		program = glCreateProgram();
		glAttachShader(program,vertex_shader);
		glAttachShader(program,fragment_shader);
		glLinkProgram(program);
		glUseProgram(program);
	}

	void GLFWRenderer::render() {
		// GL code begin
		
		// GL code goes here
		
		//make window coloured blue
		glClearColor(0.0f,0.0f,1.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//vertices
		GLfloat vertices[] = {
			-0.8f, 0.8f, 0.0f,
			-0.6f, 0.8f, 0.0f,
			-0.8f, 0.6f, 0.0f,
			-0.8f, 0.6f, 0.0f,
			-0.6f, 0.6f, 0.0f,
			-0.6f, 0.8f, 0.0f,
		};

		const GLfloat step = 0.2f;

		for (int i = 0; i < 8; i++){

			GLfloat alteredVertices[3*6];

			for (int x = 0; x < 3*6; x++){ //translate on oX axis
				if (x%3==0){ //X coordinate of vertex
					alteredVertices[x] = vertices[x] + step*i;
				}
				else{ //Y and Z coordinates
					alteredVertices[x] = vertices[x];
				}
			}

			for (int j = 0; j < 8; j++){

				for (int y = 1; y < 3*6; y+=3){ //Y coordinates are with offset 1
					alteredVertices[y] = vertices[y] - step*j;
				}

				//send vertices to be drawn
				glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,alteredVertices);
				glEnableVertexAttribArray(0);

				int myColorLocation = glGetUniformLocation(program,"myColor");

				if ((i+j)%2==1){
					glUniform4f(myColorLocation,0.0f,0.0f,0.0f,1.0f);
				}
				else{
					glUniform4f(myColorLocation,1.0f,1.0f,1.0f,1.0f);
				}

				glDrawArrays(GL_TRIANGLES,0,6);
			}
		}

		

		// GL code end
		glfwSwapBuffers(window());
	}

	bool GLFWRenderer::poll() {
		if (glfwWindowShouldClose(window())) {
			return false;
		}
		return true;
	}
}