#include "Graphics.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "SceneNode.h"
#include "Shapes.h"
#include <SDL.h>
#include <SDL_opengl.h>

namespace Temporal
{
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

	void setColor(const Color& color)
	{
		glColor4f(color.getR(), color.getG(), color.getB(), color.getA());
	}

	void Graphics::translate(const Vector& translation) const
	{
		glTranslatef(translation.getVx(), translation.getVy(), 0.0f);
	}

	void Graphics::draw(const SceneNode& sceneNode, const SpriteSheet& spritesheet, const Color& color) const
	{
		const Texture& texture = spritesheet.getTexture();
		const Hash& spriteGroupID = sceneNode.getSpriteGroupID();
		
		glPushMatrix();
		{	
			translate(sceneNode.getTranslation());
			if(sceneNode.isMirrored())
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(sceneNode.getRotation(), 0.0, 0.0, 1.0f);

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if((**i).drawBeforeParent())
					draw(**i, spritesheet, color);
			}

			if(spriteGroupID != Hash::INVALID)
			{
				glPushMatrix();
				{
					const SpriteGroup& spriteGroup = spritesheet.get(spriteGroupID);
					int spriteIndex = sceneNode.getSpriteInterpolation() == 1.0f ? spriteGroup.getSize() - 1 : static_cast<int>(spriteGroup.getSize() * sceneNode.getSpriteInterpolation());
					const Sprite& sprite = spriteGroup.get(spriteIndex);
					const AABB& texturePart = sprite.getBounds();
				
					const Size& textureSize = texture.getSize();
					float textureWidth = textureSize.getWidth();
					float textureHeight = textureSize.getHeight();

					// Images are arranged from top to bottom, and you have to include the last pixel that you don't use
					float imageLeft = texturePart.getLeft();
					float imageRight = texturePart.getRight() + 1.0f;
					float imageTop = texturePart.getBottom();
					float imageBottom = texturePart.getTop() + 1.0f;

					const float textureX0 = imageLeft / textureWidth;
					const float textureX1 = imageRight / textureWidth;
					const float textureTop = imageTop / textureHeight;
					const float textureBottom = imageBottom / textureHeight;

					glBindTexture(GL_TEXTURE_2D, texture.getID());

					setColor(color);

					Vector offset = Vector(-sprite.getOffset().getVx(), -sprite.getOffset().getVy());
					glTranslatef(offset.getVx(), offset.getVy(), 0.0f);
					GLfloat screenVertices[] = { -texturePart.getRadiusVx(), -texturePart.getRadiusVy(),
												 -texturePart.getRadiusVx(), texturePart.getRadiusVy(),
												  texturePart.getRadiusVx(), texturePart.getRadiusVy(),
												  texturePart.getRadiusVx(), -texturePart.getRadiusVy()};

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

			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				if(!(**i).drawBeforeParent())
					draw(**i, spritesheet, color);
			}
		}
		glPopMatrix();
	}

	void Graphics::draw(const AABB& rect, const Color& color) const
	{
		glBindTexture(GL_TEXTURE_2D, 0);

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
		glBindTexture(GL_TEXTURE_2D, 0);

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
		glBindTexture(GL_TEXTURE_2D, 0);

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
			const AABB& rect = (const AABB&)shape;
			draw(rect, color);
		}
		else if(shape.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg = (const Segment&)shape;
			draw(seg, color);
		}
		else if(shape.getType() == ShapeType::YABP)
		{
			const YABP& yabp = (const YABP&)shape;
			draw(yabp, color);
		}
		else
		{
			exit(1);
		}
	}
}