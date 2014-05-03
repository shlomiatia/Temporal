#ifndef FBO_H
#define FBO_H

#include "ShaderProgram.h"
#include "SpriteBatch.h"

namespace Temporal
{
	class Texture;

	class FBO
	{
	public:

		FBO() : _timeUniform(0), _time(0.0f), _texture(0), _fbo(0), _batch(_program) {}
		~FBO();

		void init(int type);
		void bind();
		void unbind();
		void draw();
	
	private:
		ShaderProgram _program;
		SpriteBatch _batch;
		const Texture* _texture;
		int _timeUniform;
		unsigned int _fbo;
		float _time;
	};

}

#endif