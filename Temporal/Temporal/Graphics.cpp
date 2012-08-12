#include "Graphics.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "SceneNode.h"
#include "Shapes.h"
#include <SDL.h>
#include <SDL_opengl.h>

namespace Temporal
{
	void setColor(const Color& color)
	{
		glColor4f(color.getR(), color.getG(), color.getB(), color.getA());
	}

	void Graphics::init(const Size& resolution, const Size& viewSize, bool fullScreen)
	{
		if ((SDL_WasInit(SDL_INIT_VIDEO) == 0) && (SDL_Init(SDL_INIT_VIDEO) != 0))
		{
			exit(1);
			// ERROR: Error Failed initializing SDL video
		}

		setVideoMode(resolution, viewSize, fullScreen);
	}

	void Graphics::setVideoMode(const Size& resolution, const Size& viewSize, bool fullScreen) const
	{
		if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1)
		{
			// ERROR: Error Failed setting SDL OpenGL double buffering attribute
			exit(1);
		}
		int flags = SDL_OPENGL;
		if (fullScreen) flags |= SDL_FULLSCREEN;

		if (SDL_SetVideoMode(static_cast<int>(resolution.getWidth()), static_cast<int>(resolution.getHeight()), BIT_DEPTH, flags) == NULL)
		{
			// ERROR: Error Failed setting video mode
			exit(1);
		}
		
		glViewport(0, 0, static_cast<int>(resolution.getWidth()), static_cast<int>(resolution.getHeight()));
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, viewSize.getWidth(), 0.0f, viewSize.getHeight(), -1.0f, 1.0f);

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
			exit(1);
		}
	}

	void Graphics::translate(const Vector& translation) const
	{
		glTranslatef(translation.getVx(), translation.getVy(), 0.0f);
	}

	void Graphics::bindTexture(unsigned int id) const
	{
		if(id != _lastTextureId)
		{
			glBindTexture(GL_TEXTURE_2D, id);
			_lastTextureId = id;
		}
	}

	void Graphics::draw(const SpriteSheet& spritesheet, const SceneNode& sceneNode, const Color& color) const
	{
		glPushMatrix();
		{	
			translate(sceneNode.getTranslation());
			if(sceneNode.isMirrored())
				glScalef(-1.0f, 1.0f, 1.0f);
			glRotatef(sceneNode.getRotation(), 0.0, 0.0, 1.0f);

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if((**i).drawBehindParent())
					draw(spritesheet, **i, color);
			}

			if(!sceneNode.isTransformOnly())
			{
				const Texture& texture = spritesheet.getTexture();
				const Hash& spriteGroupID = sceneNode.getSpriteGroupID();
				const SpriteGroup& spriteGroup = spritesheet.get(spriteGroupID);
				int spriteIndex = sceneNode.getSpriteInterpolation() == 1.0f ? spriteGroup.getSize() - 1 : static_cast<int>(spriteGroup.getSize() * sceneNode.getSpriteInterpolation());
				const Sprite& sprite = spriteGroup.get(spriteIndex);
				const AABB& texturePart = sprite.getBounds();

				Point offset(-sprite.getOffset().getVx(), -sprite.getOffset().getVy());
				draw(offset, texture, texturePart, color);
			}

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if(!(**i).drawBehindParent())
					draw(spritesheet, **i, color);
			}
		}
		glPopMatrix();
	}

	void Graphics::draw(const Point& position, const Texture& texture, const Color& color) const
	{
		draw(position, texture, AABB(texture.getSize() / 2.0f, texture.getSize()), color);
	}

	void Graphics::draw(const Point& position, const Texture& texture, const AABB& texturePart, const Color& color) const
	{
		glPushMatrix();
		{
			bindTexture(texture.getID());
			glTranslatef(position.getX(), position.getY(), 0.0f);				
			setColor(color);

			const Size& textureSize = texture.getSize();
			float textureWidth = textureSize.getWidth();
			float textureHeight = textureSize.getHeight();

			// Images are arranged from top to bottom, and you have to include the last pixel that you don't use
			float imageLeft = texturePart.getLeft();
			float imageRight = texturePart.getRight();
			float imageTop = texturePart.getBottom();
			float imageBottom = texturePart.getTop();

			const float textureLeft = imageLeft / textureWidth;
			const float textureRight = imageRight / textureWidth;
			const float textureTop = imageTop / textureHeight;
			const float textureBottom = imageBottom / textureHeight;

			GLfloat screenVertices[] = { -texturePart.getRadiusVx(), -texturePart.getRadiusVy(),
											-texturePart.getRadiusVx(), texturePart.getRadiusVy(),
											texturePart.getRadiusVx(), texturePart.getRadiusVy(),
											texturePart.getRadiusVx(), -texturePart.getRadiusVy()};

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

	void Graphics::draw(const AABB& rect, const Color& color) const
	{
		bindTexture(0);

		glPushMatrix();
		{
			setColor(color);

			glTranslatef(rect.getCenterX(), rect.getCenterY(), 0.0f);

			GLfloat vertices[] = {-rect.getRadiusVx(), -rect.getRadiusVy(),
								  -rect.getRadiusVx(), rect.getRadiusVy(),
								   rect.getRadiusVx(), rect.getRadiusVy(),
								   rect.getRadiusVx(), -rect.getRadiusVy()};
 
			glEnableClientState(GL_VERTEX_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_QUADS, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glPopMatrix();
	}

	void Graphics::draw(const YABP& slopedArea, const Color& color) const
	{
		bindTexture(0);

		glPushMatrix();
		{
			setColor(color);

			glTranslatef(slopedArea.getCenterX(), slopedArea.getCenterY(), 0.0f);

			Vector yRadius = Vector(0.0f, slopedArea.getYRadius());
			Vector plusRadius = slopedArea.getSlopedRadius() + yRadius;
			Vector minusRadius = slopedArea.getSlopedRadius() - yRadius;

			GLfloat vertices[] = {-plusRadius.getVx(), -plusRadius.getVy(),
								  -minusRadius.getVx(), -minusRadius.getVy(),
								   plusRadius.getVx(), plusRadius.getVy(),
								   minusRadius.getVx(), minusRadius.getVy()};
 
			glEnableClientState(GL_VERTEX_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_LINE_LOOP, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glPopMatrix();
	}

	void Graphics::draw(const Segment& segment, const Color& color) const
	{
		bindTexture(0);

		glPushMatrix();
		{
			setColor(color);

			glTranslatef(segment.getCenterX(), segment.getCenterY(), 0.0f);

			GLfloat vertices[] = { -segment.getRadiusVx(), -segment.getRadiusVy(), segment.getRadiusVx(), segment.getRadiusVy() };

			glEnableClientState(GL_VERTEX_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_LINES, 0, 2);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glPopMatrix();
	}

	void Graphics::draw(const Shape& shape, const Color& color) const
	{
		if(shape.getType() == ShapeType::AABB)
		{
			const AABB& rect = static_cast<const AABB&>(shape);
			draw(rect, color);
		}
		else if(shape.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg = static_cast<const Segment&>(shape);
			draw(seg, color);
		}
		else
		{
			exit(1);
		}
	}
}