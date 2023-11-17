#include <renderer.hpp>
#include <exception.hpp>

#include <memory>
#include <optional>
#include <cassert>
#include <array>
#include <iostream>
#include <string>
#include <fstream>


#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace opengles_workspace
{
	std::string GLFWRenderer::ReadShaderFile(const char* filepath){

		std::string line,src_code;
		std::ifstream ShaderFile(filepath);

		while ( getline(ShaderFile,line)){ //read line by line
			src_code += line + "\n";
		}

		ShaderFile.close(); //close file and return
		return src_code;
	}

	GLuint GLFWRenderer::LoadShader(const char* filepath, GLenum type){

		std::string shaderTemp = ReadShaderFile(filepath); //read shader from file
		const char* shaderStr = shaderTemp.c_str(); //const char required, didn't return directly one from
		//readshaderfile cuz it gave back gibberish
		GLuint shader = glCreateShader(type); //vertex or fragment shader
		glShaderSource(shader,1,&shaderStr,NULL); //send shader code to shader
		glCompileShader(shader);
		return shader;
	}

	void GLFWRenderer::setupProgram(GLuint vertex_shader, GLuint fragment_shader){

		program = glCreateProgram(); //init program

		//attach shaders
		glAttachShader(program,vertex_shader);
		glAttachShader(program,fragment_shader);

		//link shaders to program and use it
		glLinkProgram(program);
		glUseProgram(program);
	}

	void GLFWRenderer::setupTexture(unsigned int &texture, const char* filepath){

		//bind texture
		glBindTexture(GL_TEXTURE_2D, texture);

		//set texture bounding and filtering options on currently bound texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true); //necessary else texture is flipped vertically

		//load texture image
		int width, height, noChannels;
		unsigned char* data = stbi_load(filepath,&width,&height,&noChannels,0);

		if(data){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else{
			std::cout<<"Failed to load texture\n";
		}

		stbi_image_free(data); //free image data
		
	}

	void GLFWRenderer::draw(unsigned int texture){

		//bind texture for drawing
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		//positions
		glVertexAttribPointer(0, noPosValsPerVertex, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		//texture
		glVertexAttribPointer(2, noTexValsPerVertex, GL_FLOAT, GL_FALSE, stride*sizeof(GLfloat), (void*)(sizeof(float) * offsetTex));
		glEnableVertexAttribArray(2);

		glDrawElements(GL_TRIANGLES, noPrimitives*noIndicesPerPrimitive, GL_UNSIGNED_INT, 0);
	}

	GLFWRenderer::GLFWRenderer(std::shared_ptr<Context> context)
		: mContext(std::move(context))
	{
		//shader codes
		//compile shaders

		//vertex shader
		const GLuint vertex_shader = LoadShader("vertexshader.vertexshader",GL_VERTEX_SHADER);

		//fragment shader
		const GLuint fragment_shader = LoadShader("fragmentshader.fragmentshader",GL_FRAGMENT_SHADER);

		//link shaders to program
		setupProgram(vertex_shader,fragment_shader);

		//generate texture
		glGenTextures(1, &food_texture);
		setupTexture(food_texture, "../include/assets/food.png");

		glGenTextures(1, &snake_body_texture);
		setupTexture(snake_body_texture, "../include/assets/snake_body.png");
		
	}

	void GLFWRenderer::render(std::deque<Pos> snake_queue, Pos food_pos) {
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

		//get translation delta uniform
		int delta_location = glGetUniformLocation(program,"delta");

		//food position
		glUniform2f(delta_location,food_pos.x,food_pos.y);

		//draw square
		draw(food_texture);

		//queue position
		for(Pos snake_body: snake_queue){
			glUniform2f(delta_location,snake_body.x,snake_body.y);
			draw(snake_body_texture);
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