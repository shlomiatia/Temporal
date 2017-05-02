#ifndef FBO_H
#define FBO_H

#include "SpriteBatch.h"

namespace Temporal
{
	class Texture;
	class SpriteBatch;

	class FBO
	{
	public:

		FBO(SpriteBatch& batch, float resolutionDivider = 1.0f) : _batch(batch), _resolutionDivider(resolutionDivider), _texture(0), _fbo(0) {}
		~FBO();

		void init();
		void bind();
		void unbind();
		void draw();
	
	private:
		float _resolutionDivider;
		SpriteBatch& _batch;
		const Texture* _texture;
		unsigned int _fbo;

		FBO(const FBO&);
		FBO& operator=(const FBO&);
	};

}

#endif