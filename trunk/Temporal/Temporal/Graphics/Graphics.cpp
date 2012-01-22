#include "Graphics.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdlib>

namespace Temporal
{
	void Graphics::init(const Vector& resolution, const Vector& viewSize, bool fullScreen)
	{
		// init SDL video if not initialized already
		if ((SDL_WasInit(SDL_INIT_VIDEO) == 0) && (SDL_Init(SDL_INIT_VIDEO) != 0))
		{
			exit(1);
			// TODO: Error Failed initializing SDL video
		}

		// call existing methods to set window properties
		setVideoMode(resolution, viewSize, fullScreen);
	}
	void Graphics::setVideoMode(const Vector& resolution, const Vector& viewSize, bool fullScreen) const
	{
		// set video mode flags
		if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1)
		{
			// TODO: Error Failed setting SDL OpenGL double buffering attribute
			exit(1);
		}
		int flags = SDL_OPENGL;
		if (fullScreen) flags |= SDL_FULLSCREEN;

		// set video mode for SDL
		if (SDL_SetVideoMode((int)resolution.getWidth(), (int)resolution.getHeight(), BIT_DEPTH, flags) == NULL)
		{
			// TODO: Error Failed setting video mode
			exit(1);
		}
		// init GL stuff
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		glDepthMask(GL_FALSE);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glViewport(0, 0, (int)resolution.getWidth(), (int)resolution.getHeight());
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, viewSize.getX(), 0, viewSize.getY(), 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		validate();
	}

	void Graphics::setTitle(const char* title) const
	{
		SDL_WM_SetCaption(title, title);
	}

	void Graphics::dispose(void) const
	{
		SDL_VideoQuit();
	}
	void Graphics::prepareForDrawing(void) const
	{
		// Clear the buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// Set translation
		glLoadIdentity();
		glTranslatef(_translation.getX(), _translation.getY(), 0.0f);
	}

	void Graphics::finishDrawing(void) const
	{
		// Swap to the drawn buffer
		SDL_GL_SwapBuffers();

		// Check if exception was thrown while drawing
		validate();
	}

	void Graphics::validate(void) const
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			// TODO: Error OpenGL error
			exit(1);
		}
	}

	void setColor(const Color& color)
	{
		glColor3f(color.getR(), color.getG(), color.getB());
	}

	void Graphics::drawTexture(const Texture& texture, const Rect& texturePart, const Vector& screenLocation, bool mirrored) const
	{
		float textureWidth = texture.getSize().getWidth() - 1;
		float textureHeight = texture.getSize().getHeight() - 1;

		const float textureX0 = (!mirrored ? texturePart.getGraphicsLeft() : texturePart.getGraphicsRight()) / textureWidth;
		const float textureX1 = (!mirrored ? texturePart.getGraphicsRight() : texturePart.getGraphicsLeft()) / textureWidth;
		const float textureTop = (texturePart.getGraphicsTop()) / textureHeight;
		const float textureBottom = (texturePart.getGraphicsBottom()) / textureHeight;

		glBindTexture(GL_TEXTURE_2D, texture.getID());

		glPushMatrix();
		{
			// set opengl matrix & color properties
			glTranslatef(screenLocation.getX(), screenLocation.getY(), 0.0f);
			glColor3f(1.0f, 1.0f, 1.0f);
 
			GLfloat screenVertices[] = { -texturePart.getOffsetX(), -texturePart.getOffsetY(),
										 -texturePart.getOffsetX(), texturePart.getOffsetY(),
										  texturePart.getOffsetX(), texturePart.getOffsetY(),
										  texturePart.getOffsetX(), -texturePart.getOffsetY() };

			GLfloat textureVertices[] = { textureX0, textureBottom,
										  textureX0, textureTop,
										  textureX1, textureTop,
										  textureX1, textureBottom };
 
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, screenVertices);
			glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
 
			glDrawArrays(GL_QUADS, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glPopMatrix();
	}

	void Graphics::drawRect(const Rect& rect, const Color& color) const
	{
		setColor(color);
		glPushMatrix();
		{
			
			glTranslatef(rect.getCenterX(), rect.getCenterY(), 0.0f);

			GLfloat vertices[] = {-rect.getOffsetX(), -rect.getOffsetY(),
								  -rect.getOffsetX(), rect.getOffsetY(),
								   rect.getOffsetX(), rect.getOffsetY(),
								   rect.getOffsetX(), -rect.getOffsetY()};
 
			glEnableClientState(GL_VERTEX_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_LINE_LOOP, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glPopMatrix();
	}
}