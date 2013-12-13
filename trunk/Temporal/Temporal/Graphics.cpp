#include "Graphics.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "SceneNode.h"
#include "Shapes.h"
#include "Settings.h"
#include <SDL.h>
#include <SDL_opengl.h>

namespace Temporal
{
	void setColor(const Color& color)
	{
		glColor4f(color.getR(), color.getG(), color.getB(), color.getA());
	}

	void Graphics::init(const Settings& settings)
	{
		_resolution = settings.getResolution();
		float logicalViewWidth = settings.getViewY() * _resolution.getX() / _resolution.getY();
		_logicalView = Vector(logicalViewWidth, settings.getViewY());

		if ((SDL_WasInit(SDL_INIT_VIDEO) == 0) && (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0))
		{
			abort();
			// ERROR: Error Failed initializing SDL video
		}

		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

		if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1)
		{
			// ERROR: Error Failed setting SDL OpenGL double buffering attribute
			abort();
		}
		int flags = SDL_OPENGL;
		if (settings.isFullscreen()) flags |= SDL_FULLSCREEN;

		if (!SDL_SetVideoMode(static_cast<int>(settings.getResolution().getX()), static_cast<int>(settings.getResolution().getY()), BIT_DEPTH, flags))
		{
			// ERROR: Error Failed setting video mode
			abort();
		}
		
		glViewport(0, 0, static_cast<int>(settings.getResolution().getX()), static_cast<int>(settings.getResolution().getY()));

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, _logicalView.getX(), 0.0f, _logicalView.getY(), -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		validate();
	}

	void Graphics::setTitle(const char* title) const
	{
		SDL_WM_SetCaption(title, title);
	}

	void Graphics::dispose() const
	{
		SDL_VideoQuit();
	}

	void Graphics::prepareForDrawing() const
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
	}

	void Graphics::finishDrawing() const
	{
		SDL_GL_SwapBuffers();
		validate();
	}

	void Graphics::validate() const
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			// ERROR: Error OpenGL error
			abort();
		}
	}

	void Graphics::translate(const Vector& translation) const
	{
		glTranslatef(translation.getX(), translation.getY(), 0.0f);
	}

	void Graphics::bindTexture(unsigned int id)
	{
		if(id != _lastTextureId || id == 0)
		{
			glBindTexture(GL_TEXTURE_2D, id);
			_lastTextureId = id;
		}
	}

	void Graphics::draw(const SceneNode& sceneNode, const SpriteSheet& spritesheet, const Color& color)
	{
		glPushMatrix();
		{	
			if(sceneNode.isFlip())
				glScalef(-1.0f, 1.0f, 1.0f);
			glScalef(sceneNode.getScale().getX(), sceneNode.getScale().getY(), 1.0f);
			translate(sceneNode.getTranslation());
			glRotatef(sceneNode.getRotation(), 0.0, 0.0, 1.0f);

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if((**i).drawBehindParent())
					draw(**i, spritesheet, color);
			}

			if(!sceneNode.isTransformOnly())
			{
				const Texture& texture = spritesheet.getTexture();
				Hash spriteGroupID = sceneNode.getSpriteGroupId();
				const SpriteGroup& spriteGroup = spritesheet.get(spriteGroupID);
				float spriteInterpolation = sceneNode.getSpriteInterpolation();
				int spriteIndex;
				if(spriteInterpolation < 0.0f)
					spriteIndex = 0;
				else if(spriteInterpolation >= 1.0f)
					spriteIndex = spriteGroup.getSize() - 1;
				else
					spriteIndex = static_cast<int>(spriteGroup.getSize() * sceneNode.getSpriteInterpolation());
				const Sprite& sprite = spriteGroup.get(spriteIndex);
				const AABB& texturePart = sprite.getBounds();

				Vector offset(-sprite.getOffset());
				draw(offset, texture, texturePart, color);
			}

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if(!(**i).drawBehindParent())
					draw(**i, spritesheet, color);
			}
		}
		glPopMatrix();
	}

	void Graphics::draw(const Vector& position, const Texture& texture, const Color& color)
	{
		draw(position, texture, AABB(texture.getSize() / 2.0f, texture.getSize()), color);
	}

	void Graphics::draw(const Vector& position, const Texture& texture, const AABB& texturePart, const Color& color)
	{
		glPushMatrix();
		{
			bindTexture(texture.getID());
			glTranslatef(position.getX(), position.getY(), 0.0f);				
			setColor(color);

			const Vector& textureSize = texture.getSize();
			float textureWidth = textureSize.getX();
			float textureHeight = textureSize.getY();

			// Images are arranged from top to bottom, and you have to include the last pixel that you don't use
			float imageLeft = texturePart.getLeft();
			float imageRight = texturePart.getRight();
			float imageTop = texturePart.getBottom();
			float imageBottom = texturePart.getTop();

			const float textureLeft = imageLeft / textureWidth;
			const float textureRight = imageRight / textureWidth;
			const float textureTop = imageTop / textureHeight;
			const float textureBottom = imageBottom / textureHeight;

			GLfloat screenVertices[] = { -texturePart.getRadiusX(), -texturePart.getRadiusY(),
											-texturePart.getRadiusX(), texturePart.getRadiusY(),
											texturePart.getRadiusX(), texturePart.getRadiusY(),
											texturePart.getRadiusX(), -texturePart.getRadiusY()};

			GLfloat textureVertices[] = { textureLeft, textureBottom,
											textureLeft, textureTop,
											textureRight, textureTop,
											textureRight, textureBottom };
 
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

	void Graphics::draw(const AABB& rect, const Color& color, bool fill)
	{
		bindTexture(0);
		setColor(color);
		GLfloat vertices[] = {rect.getLeft(), rect.getBottom(),
							  rect.getLeft(), rect.getTop(),
							  rect.getRight(), rect.getTop(),
							  rect.getRight(), rect.getBottom()};
 
		glEnableClientState(GL_VERTEX_ARRAY);
 
		glVertexPointer(2, GL_FLOAT, 0, vertices);
 
		glDrawArrays(fill ? GL_QUADS : GL_LINE_LOOP, 0, 4);
 
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void Graphics::draw(const AABB& rect, const Color& color1, const Color& color2)
	{
		bindTexture(0);
		GLfloat vertices[] = {rect.getLeft(), rect.getBottom(),
							  rect.getLeft(), rect.getTop(),
							  rect.getRight(), rect.getTop(),
							  rect.getRight(), rect.getBottom()};

		GLfloat colors[] = {color2.getR(), color2.getG(), color2.getB() ,
							color1.getR(), color1.getG(), color1.getB(),
							color1.getR(), color1.getG(), color1.getB(),
							color2.getR(), color2.getG(), color2.getB() };
 
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
 
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glColorPointer(3, GL_FLOAT, 0, colors);
 
		glDrawArrays(GL_QUADS, 0, 4);
 
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	void Graphics::draw(const OBB& obb, const Color& color, bool fill)
	{
		bindTexture(0);

		setColor(color);

		GLfloat vertices[8];
		int j = 0;
		for(int i = 0; i < 2; ++i)
		{
			for(Axis::Enum axis = Axis::Y; axis >= Axis::X; --axis)
			{
				Vector point = obb.getPoint(axis, i != 0);
				vertices[j*2] = point.getX();
				vertices[j*2+1] = point.getY();
				++j;
			}
		}
		glEnableClientState(GL_VERTEX_ARRAY);
 
		glVertexPointer(2, GL_FLOAT, 0, vertices);
 
		glDrawArrays(fill ? GL_QUADS : GL_LINE_LOOP, 0, 4);
 
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void Graphics::draw(const Segment& segment, const Color& color)
	{
		bindTexture(0);

		glPushMatrix();
		{
			setColor(color);

			glTranslatef(segment.getCenterX(), segment.getCenterY(), 0.0f);

			GLfloat vertices[] = { -segment.getRadiusX(), -segment.getRadiusY(), segment.getRadiusX(), segment.getRadiusY() };

			glEnableClientState(GL_VERTEX_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_LINES, 0, 2);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glPopMatrix();
	}
}