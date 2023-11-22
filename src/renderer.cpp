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
	/**
	 * readShaderFile: function to get contents of the shader file at a given path
	 * @param filepath: the path to the shader file to be read
	 * @return: a string containing all of the lines from the shader file
	*/
	std::string GLFWRenderer::readShaderFile(const char* filepath){

		std::string line,src_code;
		std::ifstream shader_file(filepath);
		if (!shader_file.fail()){
			while (getline(shader_file,line)){ //read line by line
				src_code += line + "\n";
			}

			shader_file.close(); //close file and return
		}
		else{
			std::cout<<"Did not read the shader\n";
		}
		
		return src_code;
	}

	/**
	 * loadShader: function to load a shader's source code and compile it
	 * @param filepath: the path to the shader file to be read (SEE ReadShaderFile)
	 * @param type: the type of the shader to be loaded (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
	 * @return: the handler of the loaded and compiled shader file
	*/
	GLuint GLFWRenderer::loadShader(const char* filepath, GLenum type){

		std::string shader_temp = readShaderFile(filepath); //read shader from file
		const char* shader_str = shader_temp.c_str(); //const char required, didn't return directly one from
		//readshaderfile cuz it returned some unclear data
		GLuint shader = glCreateShader(type); //vertex or fragment shader

		if (shader == 0){
			std::cout<<"Did not create the shader\n";
			return 0;
		}
		glShaderSource(shader,1,&shader_str,NULL); //send shader code to shader
		glCompileShader(shader);
		GLint compiled;
		glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
		if (!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen > 1)
			{
				char* infoLog = (char*) malloc (sizeof(char) * infoLen );
				glGetShaderInfoLog( shader, infoLen, NULL, infoLog );
				std::cout<<"Error compiling shader: "<<infoLog<<'\n';
				free (infoLog);
			}
			glDeleteShader (shader);
			return 0;
		}

		return shader;
	}

	/**
	 * setupProgram: function to setup the program to be used by the application
	 * @param vertex_shader: handler to the vertex shader of the program
	 * @param fragment_shader: handler to the fragment shader of the program
	*/
	void GLFWRenderer::setupProgram(GLuint vertex_shader, GLuint fragment_shader){

		program = glCreateProgram(); //init program
		if (program == 0){
			std::cout<<"Error creating program\n";
		}

		//attach shaders
		glAttachShader(program,vertex_shader);
		glAttachShader(program,fragment_shader);

		//link shaders to program and use it
		glLinkProgram(program);

		GLint linked;
		// Check the link status
		glGetProgramiv (program, GL_LINK_STATUS, &linked);
		if (!linked){
			GLint infoLen = 0;
			glGetProgramiv ( program, GL_INFO_LOG_LENGTH, &infoLen );
			if ( infoLen > 1 )
			{
				char* infoLog = (char*) malloc (sizeof(char) * infoLen);
				glGetProgramInfoLog ( program, infoLen, NULL, infoLog);
				std::cout<<"Error linking program: "<<infoLog<<'\n';
				free(infoLog);
			}
		}
		else{
			glUseProgram(program);
		}
	}

	/**
	 * setupTexture: a function to setup a texture and return its handle given its file path
	 * @param filepath: the path to the file used as an image
	 * @return: the handle to the texture
	*/
	unsigned int GLFWRenderer::setupTexture(const char* filepath){

		unsigned int texture;

		//bind texture
		glGenTextures(1, &texture);
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
		
		return texture;
	}

	/**
	 * draw: a function to draw a square with a given texture
	 * @param texture: the handler to the texture to be used
	*/
	void GLFWRenderer::draw(unsigned int texture){

		//bind texture for drawing
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		//positions
		glVertexAttribPointer(0, NO_POS_VALS_PER_VERTEX, GL_FLOAT, GL_FALSE, STRIDE*sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		//texture
		glVertexAttribPointer(2, NO_TEX_VALS_PER_VERTEX, GL_FLOAT, GL_FALSE, STRIDE*sizeof(GLfloat), (void*)(sizeof(float) * OFFSET_TEX));
		glEnableVertexAttribArray(2);

		glDrawElements(GL_TRIANGLES, NO_PRIMITIVES*NO_INDICES_PER_PRIMITIVE, GL_UNSIGNED_INT, 0);
	}

	/**
	 * setData: a function to set data needed for drawing
	 * @param snake: a snake type object, whose queue contains upper left corner positions of drawn squares
	 * @param food_pos: the upper left corner position of the food object
	*/
	void GLFWRenderer::setData(Snake snake, Pos food_pos){
		this->snake = snake;
		this->food_pos = food_pos;
	}

	GLFWRenderer::GLFWRenderer(std::shared_ptr<Context> context)
		: mContext(std::move(context))
	{
		//shader codes
		//compile shaders

		//vertex shader
		const GLuint vertex_shader = loadShader("vertexshader.vertexshader",GL_VERTEX_SHADER);

		//fragment shader
		const GLuint fragment_shader = loadShader("fragmentshader.fragmentshader",GL_FRAGMENT_SHADER);

		//link shaders to program
		setupProgram(vertex_shader,fragment_shader);

		//generate texture
		food_texture = setupTexture("../include/assets/food.png");
		snake_body_texture = setupTexture("../include/assets/snake_body.png");
		
	}

	void GLFWRenderer::render() {
		// GL code begin
		
		// GL code goes here
		
		//make window coloured blue if snake is alive else red
		snake.isAlive() ? glClearColor(0.0f,0.0f,0.4f,0.0f) : glClearColor(0.4f,0.0f,0.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//vertices
		GLfloat vertices[] = { //reference square for all pieces
			//pos coords (XYZ)		//tex coords(S,T)
			0.0f, 0.0f, 0.0f,		0.0f, 1.0f, //upper left corner
			STEP, 0.0f, 0.0f,		1.0f, 1.0f, //upper right corner
			0.0f, -STEP, 0.0f,		0.0f, 0.0f, //lower left corner
			STEP, -STEP, 0.0f,		1.0f, 0.0f, //lower right corner
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*NO_VALS_PER_VERTEX*NO_VERTICES, vertices, GL_STATIC_DRAW);

		//CREATE IBO
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*NO_PRIMITIVES*NO_INDICES_PER_PRIMITIVE, indices, GL_STATIC_DRAW);

		//get translation delta uniform
		int delta_location = glGetUniformLocation(program,"delta");

		//food position
		glUniform2f(delta_location,food_pos.x,food_pos.y);

		//draw square
		draw(food_texture);

		//queue position
		for(Pos snake_body: snake.getQueue()){
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