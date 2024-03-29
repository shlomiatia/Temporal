#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

namespace Temporal
{
	class ShaderProgram
	{
	public:
		ShaderProgram() : _program(0) {}
		void init(const char * vertexShaderFile, const char * fragmentShaderFile);
		~ShaderProgram();

		unsigned int getId() const { return _program; }
		int getAttribute(const char* key);
		int getUniform(const char* key);
		void setUniform(int uniform, int value);
		void setUniform(int uniform, float value);
		void setUniform(int uniform, float* value);

	private:
		unsigned int createShader(unsigned int shaderType, const char* shaderData);
		unsigned int loadShader(unsigned int shaderType, const char* shaderFile);
		unsigned int createProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId);

		unsigned int _program;

		ShaderProgram(const ShaderProgram&);
		ShaderProgram& operator=(const ShaderProgram&);
	};
}

#endif