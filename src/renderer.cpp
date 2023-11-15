#include <renderer.hpp>
#include <exception.hpp>

#include <memory>
#include <optional>
#include <cassert>
#include <array>
#include <iostream>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
		"layout(location = 2) in vec2 vTextureCoord; \n"
		"out vec2 fTextureCoord; \n"
		"void main() \n"
		"{ \n"
		" gl_Position = vPosition; \n"
		" fTextureCoord = vTextureCoord; \n"
		"} \n";

		//fragment shader
		const char* fShaderStr =
		"#version 300 es \n"
		"precision mediump float; \n"
		"in vec2 fTextureCoord; \n"
		"uniform sampler2D mySampler; \n"
		"out vec4 fragColor; \n"
		"void main() \n"
		"{ \n"
		" fragColor = texture(mySampler, fTextureCoord); \n"
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

		//generate texture
		unsigned int textures[2]; //just food and snake body
		glGenTextures(2, textures);

		//food texture
		glActiveTexture(GL_TEXTURE0); //first texture
		glBindTexture(GL_TEXTURE_2D, textures[0]);

		//set texture bounding and filtering options on currently bound texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true); //necessary else texture is flipped vertically
		//load texture image
		int width, height, noChannels;
		unsigned char* food_data = stbi_load("../include/assets/food.png",&width,&height,&noChannels,0);

		if(food_data){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, food_data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else{
			std::cout<<"Failed to load texture\n";
		}

		stbi_image_free(food_data); //free image data

		
		//snake body texture
		glActiveTexture(GL_TEXTURE1); //second texture
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		//set texture bounding and filtering options on currently bound texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true); //necessary else texture is flipped vertically
		//load texture image
		unsigned char* snake_body_data = stbi_load("../include/assets/snake_body.png",&width,&height,&noChannels,0);

		if(snake_body_data){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, snake_body_data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else{
			std::cout<<"Failed to load texture\n";
		}

		stbi_image_free(snake_body_data); //free image data

		
	}

	void GLFWRenderer::render() {
		// GL code begin
		
		// GL code goes here
		
		//make window coloured blue
		glClearColor(0.0f,0.0f,1.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//vertices
		GLfloat vertices[] = { //reference square for all pieces
			//pos coords (XYZ)		//tex coords(S,T)
			0.0f, 0.0f, 0.0f,		0.0f, 1.0f, //upper left corner
			0.2f, 0.0f, 0.0f,		1.0f, 1.0f, //upper right corner
			0.0f, -0.2f, 0.0f,		0.0f, 0.0f, //lower left corner
			0.2f, -0.2f, 0.0f,		1.0f, 0.0f, //lower right corner
		};
		
		//indices
		unsigned int indices[] = {
			0,1,2,
			1,2,3,
		};

		//constants
		const int stride = 5;
		const int noVertices = 4;
		const int noValsPerVertex = 5;
		const int noPosValsPerVertex = 3;
		const int noTexValsPerVertex = 2;
		const int offsetTex = 3;
		const int noPrimitives = 2;
		const int noIndicesPerPrimitive = 3;
		

		//Create VAO
		glGenVertexArrays(1,&VAO);
		glBindVertexArray(VAO);

		//Create VBO
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*noValsPerVertex*noVertices, vertices, GL_STATIC_DRAW);

		//CREATE IBO
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*noPrimitives*noIndicesPerPrimitive, indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, noPosValsPerVertex, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, noTexValsPerVertex, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(sizeof(float) * offsetTex));
		glEnableVertexAttribArray(2);

		glDrawElements(GL_TRIANGLES, noPrimitives*noIndicesPerPrimitive, GL_UNSIGNED_INT, 0);

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