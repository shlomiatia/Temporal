#include "Texture.h"
#include "Graphics.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cstdlib>

namespace Temporal
{
	const Texture* const Texture::load(const char* path)
	{
		SDL_Surface* image(IMG_Load(path));
		if (!image)
		{
			exit(1);
			// TODO: Failed loading image
		}
		GLenum texture_format;
		if (image->format->BytesPerPixel == 4)
		{
			if (image->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		}
		else if (image->format->BytesPerPixel == 3)
		{
			if (image->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}
		else
		{
			exit(1);
			// TODO: Invalid bytes per pixel
		}

		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, texture_format, GL_UNSIGNED_BYTE, image->pixels);
		Vector size((float)image->w, (float)image->h);
		const Texture* const result(new Texture(id, size));

		Graphics::get().validate();
		SDL_FreeSurface(image);
		return result;
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &_id);
	}
}