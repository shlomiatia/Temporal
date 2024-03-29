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
		SDL_Surface *surface = NULL;
	
		if (!(surface = IMG_Load(path)))
			return 0;

		char colors = surface->format->BytesPerPixel;
		int format;
		// get the image format
		if (colors == 4) {
			if (surface->format->Rmask == 0x000000ff) {
				format = GL_RGBA;
			} else {
				format = GL_BGRA;
			}
		} else if (colors == 3) {
			if (surface->format->Rmask == 0x000000ff) {
				format = GL_RGB;
			} else {
				format = GL_BGR;
			}
		}
		unsigned int id;
		// generate the gl texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
 
		// set filter parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
		int width = surface->w;
		int height = surface->h;
	
		// send the loaded binary into open gl
		glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
		Graphics::get().validate();
		SDL_FreeSurface(surface);
		return new Texture(id, Vector(width, height));
	}

	const Texture* Texture::load(const Vector& size, bool greyscale)
	{
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int internalFormat = GL_RGBA;
		int format = greyscale ? GL_RED : GL_RGBA;
		int width = static_cast<int>(size.getX());
		int height = static_cast<int>(size.getY());
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, 0);

		const Texture* result(new Texture(id, size));
		Graphics::get().validate();
		return result;
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &_id);
	}
}