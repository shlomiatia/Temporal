#include "FBO.h"
#include "GL/glew.h"
#include "Graphics.h"
#include "Texture.h"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Temporal
{
	void FBO::init(int type)
	{
		_program.init("resources/shaders/v.glsl", "resources/shaders/xf.glsl");

		glm::mat4 projection = glm::ortho(0.0f, Graphics::get().getResolution().getX(), Graphics::get().getResolution().getY(), 0.0f, -1.0f, 1.0f);
		_program.setUniform(_program.getUniform("u_projection"), glm::value_ptr(projection));

		_batch.init();

		int typeUniform = _program.getUniform("u_type");
		_timeUniform = _program.getUniform("u_time");
		_program.setUniform(typeUniform, type);
		
		_texture = Texture::load(Graphics::get().getResolution() / 10.0f);
 
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
		glViewport(0, 0, Graphics::get().getResolution().getX() / 10.0f, Graphics::get().getResolution().getY() / 10.0f);
	}

	void FBO::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Graphics::get().getResolution().getX(), Graphics::get().getResolution().getY());
	}

	void FBO::draw()
	{
		_time += 1.0f/60.0f;
		_program.setUniform(_timeUniform, _time);
		_batch.begin();
		_batch.add(_texture, Graphics::get().getResolution() / 2.0f, AABB::Zero, Color::White, 0.0f, Vector::Zero, Vector(1.0f, 1.0f), false, false, Graphics::get().getResolution() / 2.0f);
		_batch.end();
	}
}