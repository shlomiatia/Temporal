#include "ShaderProgram.h"
#include <GL/glew.h>
#include "Graphics.h"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>

namespace Temporal
{
	ShaderProgram::~ShaderProgram() 
	{
		glDeleteProgram(_program); 
	}

	void ShaderProgram::init(const char* vertexShaderFile, const char* fragmentShaderFile)
	{
		unsigned int vertexShaderId = loadShader(GL_VERTEX_SHADER, vertexShaderFile);
		Graphics::get().validate();
		unsigned int fragmentShaderId = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFile);
		Graphics::get().validate();

		_program = createProgram(vertexShaderId, fragmentShaderId);

		glUseProgram(_program);

		glm::mat4 projection = glm::ortho(0.0f, Graphics::get().getLogicalView().getX(), 0.0f, Graphics::get().getLogicalView().getY(), -1.0f, 1.0f);
		glUniformMatrix4fv(getUniform("u_projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(0);
	}

	int ShaderProgram::getAttribute(const char* key)
	{
		int result = glGetAttribLocation(_program, key);
		if(result == -1)
			abort();
		return result;
	}

	int ShaderProgram::getUniform(const char* key)
	{
		int result = glGetUniformLocation(_program, key);
		if(result == -1)
			abort();
		return result;
	}

	void ShaderProgram::setUniform(int uniform, int value)
	{
		glUseProgram(_program);
		glUniform1i(uniform, value);
		glUseProgram(0);
	}

	void ShaderProgram::setUniform(int uniform, float value)
	{
		glUseProgram(_program);
		glUniform1f(uniform, value);
		glUseProgram(0);
	}

	unsigned int ShaderProgram::createShader(unsigned int shaderType, const char* shaderData)
	{
		unsigned int shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderData, NULL);
		glCompileShader(shader);

		int status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			int infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			char * infoLog = new char[infoLogLength + 1];
			glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
			abort();
		}

		return shader;
	}

	unsigned int ShaderProgram::loadShader(unsigned int shaderType, const char* shaderFile)
	{
		std::ifstream shaderFileStream(shaderFile);
		std::stringstream shaderDataStream;
		shaderDataStream << shaderFileStream.rdbuf();
		shaderFileStream.close();

		return createShader(shaderType, shaderDataStream.str().c_str());
	}

	unsigned int ShaderProgram::createProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId)
	{
		unsigned int program = glCreateProgram();

		glAttachShader(program, vertexShaderId);
		glAttachShader(program, fragmentShaderId);

		glLinkProgram(program);

		int status;
		glGetProgramiv (program, GL_LINK_STATUS, &status);
		if (status == 0)
		{
			int infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
			abort();
		}

		glDetachShader(program, vertexShaderId);
		glDetachShader(program, fragmentShaderId);

		return program;
	}
}