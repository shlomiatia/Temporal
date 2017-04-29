#include "FBO.h"
#include "GL/glew.h"
#include "Graphics.h"
#include "Texture.h"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Temporal
{
	void FBO::init()
	{
		_texture = Texture::load(Graphics::get().getResolution() / _resolutionDivider);
 
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getID(), 0);
		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
			abort();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	FBO::~FBO()
	{
		delete _texture;
		glDeleteFramebuffers(1, &_fbo);
	}

	void FBO::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, Graphics::get().getResolution().getX() / _resolutionDivider, Graphics::get().getResolution().getY() / _resolutionDivider);
	}

	void FBO::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Graphics::get().getResolution().getX(), Graphics::get().getResolution().getY());
	}

	void FBO::draw()
	{
		_batch.begin();
		_batch.add(_texture, Graphics::get().getResolution() / 2.0f, AABB::Zero, Color::White, 0.0f, Vector::Zero, Vector(1.0f, 1.0f), false, false, Graphics::get().getResolution() / 2.0f, true);
		_batch.end();
	}
}