#include "Graphics.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdlib>

namespace Temporal
{
	void Graphics::init(const Vector& resolution, const Vector& viewSize, bool fullScreen)
	{
		if ((SDL_WasInit(SDL_INIT_VIDEO) == 0) && (SDL_Init(SDL_INIT_VIDEO) != 0))
		{
			exit(1);
			// ERROR: Error Failed initializing SDL video
		}

		setVideoMode(resolution, viewSize, fullScreen);
	}
	void Graphics::setVideoMode(const Vector& resolution, const Vector& viewSize, bool fullScreen) const
	{
		if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1)
		{
			// ERROR: Error Failed setting SDL OpenGL double buffering attribute
			exit(1);
		}
		int flags = SDL_OPENGL;
		if (fullScreen) flags |= SDL_FULLSCREEN;

		if (SDL_SetVideoMode((int)resolution.getWidth(), (int)resolution.getHeight(), BIT_DEPTH, flags) == NULL)
		{
			// ERROR: Error Failed setting video mode
			exit(1);
		}
		
		glViewport(0, 0, (int)resolution.getWidth(), (int)resolution.getHeight());
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, viewSize.getX(), 0.0f, viewSize.getY(), -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

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
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();
		glTranslatef(_translation.getX(), _translation.getY(), 0.0f);
	}

	void Graphics::finishDrawing(void) const
	{
		SDL_GL_SwapBuffers();

		validate();
	}

	void Graphics::validate(void) const
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			// ERROR: Error OpenGL error
			exit(1);
		}
	}

	void setColor(const Color& color)
	{
		glColor4f(color.getR(), color.getG(), color.getB(), 1.0f);
	}

	void Graphics::drawTexture(const Texture& texture, const Rect& texturePart, const Vector& screenLocation, bool mirrored, float rotation) const
	{
		float textureWidth = texture.getSize().getWidth();
		float textureHeight = texture.getSize().getHeight();

		const float textureX0 = (!mirrored ? texturePart.getGraphicsLeft() : texturePart.getGraphicsRight()) / textureWidth;
		const float textureX1 = (!mirrored ? texturePart.getGraphicsRight() : texturePart.getGraphicsLeft()) / textureWidth;
		const float textureTop = (texturePart.getGraphicsTop()) / textureHeight;
		const float textureBottom = (texturePart.getGraphicsBottom()) / textureHeight;

		glBindTexture(GL_TEXTURE_2D, texture.getID());

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glPushMatrix();
		{	
			glTranslatef(screenLocation.getX(), screenLocation.getY(), 0.0f);
			glRotatef(rotation, 0.0, 0.0, 1.0f);

			GLfloat screenVertices[] = { -texturePart.getOffsetX(), -texturePart.getOffsetY(),
										 -texturePart.getOffsetX(), texturePart.getOffsetY(),
										  texturePart.getOffsetX(), texturePart.getOffsetY(),
										  texturePart.getOffsetX(), -texturePart.getOffsetY()};

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
		glBindTexture(GL_TEXTURE_2D, 0);

		glPushMatrix();
		{
			setColor(color);

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
	void Graphics::drawLine(const Vector& p1, const Vector& p2, const Color& color) const
	{
		glBindTexture(GL_TEXTURE_2D, 0);

		setColor(color);

		GLfloat vertices[] = { p1.getX(), p1.getY(), p2.getX(), p2.getY() };

		glEnableClientState(GL_VERTEX_ARRAY);
 
		glVertexPointer(2, GL_FLOAT, 0, vertices);
 
		glDrawArrays(GL_LINES, 0, 2);
 
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}