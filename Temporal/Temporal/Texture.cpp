#include "Texture.h"
#include "Graphics.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cstdlib>

namespace Temporal
{
	const Texture* Texture::load(const char* path)
	{
		SDL_Surface* image(IMG_Load(path));
		if (!image)
		{
			abort();
			// ERROR: Failed loading image
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
			abort();
			// ERROR: Invalid bytes per pixel
		}

		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, texture_format, GL_UNSIGNED_BYTE, image->pixels);
		Vector size(static_cast<float>(image->w), static_cast<float>(image->h));
		const Texture* result(new Texture(id, size));

		Graphics::get().validate();
		SDL_FreeSurface(image);
		return result;
	}

	const Texture* Texture::load(const Vector& size)
	{
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned int* data = new unsigned int[((static_cast<int>(size.getX()) * static_cast<int>(size.getY()))* 4 * sizeof(unsigned int))];
		glTexImage2D(GL_TEXTURE_2D, 0, 4, static_cast<int>(size.getX()), static_cast<int>(size.getY()), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		const Texture* result(new Texture(id, size));
		Graphics::get().validate();
		delete data;
		return result;
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &_id);
	}
}