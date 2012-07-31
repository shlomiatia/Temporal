#include "Graphics.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "SceneNode.h"
#include "Shapes.h"
#include <SDL.h>
#include <SDL_opengl.h>

#include "Math.h"
#include "Grid.h"
#include "StaticBody.h"

namespace Temporal
{
	static const Texture* _texture;
	void Graphics::init(const Size& resolution, const Size& viewSize, bool fullScreen)
	{
		if ((SDL_WasInit(SDL_INIT_VIDEO) == 0) && (SDL_Init(SDL_INIT_VIDEO) != 0))
		{
			exit(1);
			// ERROR: Error Failed initializing SDL video
		}

		setVideoMode(resolution, viewSize, fullScreen);
		_texture = Texture::load(viewSize);
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

	bool shadows(void* caller, void* data, const StaticBody& staticBody)
	{
		if(!staticBody.isCover())
		{
			float shadowSize = 10000.0f;
			const Vector& lightCenter = *(const Vector*)data;
			const Segment& segment = (const Segment&)staticBody.getShape();
			Point leftPoint = segment.getLeftPoint();
			Point rightPoint = segment.getRightPoint();
			Vector vector1 = Vector(leftPoint - lightCenter).normalize();
			Vector vector2 = Vector(rightPoint - lightCenter).normalize();

			float vertices[8];
			
			vertices[0] = leftPoint.getX() + vector1.getVx() * shadowSize;
			vertices[1] = leftPoint.getY() + vector1.getVy() * shadowSize;
			vertices[2] = leftPoint.getX();
			vertices[3] = leftPoint.getY();
			vertices[4] = rightPoint.getX();
			vertices[5] = rightPoint.getY();
			vertices[6] = rightPoint.getX() + vector2.getVx() * shadowSize;
			vertices[7] = rightPoint.getY() + vector2.getVy() * shadowSize;

			glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_QUADS, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		return true;
	}

	void Graphics::light(const Point& playerLight) const 
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Point lights[2] = { playerLight, Point(_texture->getSize().getWidth() / 2.0f, _texture->getSize().getHeight() / 2.0) };
		glBlendFunc(GL_DST_ALPHA, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, 0);
		for(int j = 0; j < 2; ++j)
		{
			const Point& light = lights[j == 0 ? 0 : 1];
			float radius = _texture->getSize().getHeight() / 2.0f;

			glDisable(GL_BLEND);
			glColorMask(false, false, false, true);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glColorMask(true, true, true, true);

			glColorMask(false, false, false, true);
			glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
			Grid::get().iterateTiles(AABB(light.getX(), light.getY(), radius * 2, radius * 2), 0, NULL, (void*)&light, shadows);
			glColorMask(true, true, true, true);
			glEnable(GL_BLEND);
			glPushMatrix();
			{
				glTranslatef(light.getX(), light.getY(), 0.0f);

				static const int parts = 32;

				float vertices[(parts + 1) * 2];
				float colors[(parts + 1) * 4] = { 0.5f, 0.5f, 0.5f, 1.0f };

				vertices[0] = 0.0f;
				vertices[1] = 0.0f;

				for(int i = 0; i < parts; ++i)
				{
					float angle = (PI * 2) * ((float)i / ((float)(parts - 1)));
					vertices[i*2 + 2] = radius * cos(angle);
					vertices[i*2 + 3] = radius * sin(angle);
				}

				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);

				glVertexPointer(2, GL_FLOAT, 0, vertices);
				glColorPointer(4, GL_FLOAT, 0, colors);
 
				glDrawArrays(GL_TRIANGLE_FAN, 0, parts + 1);
 
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
			}
			glPopMatrix();
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, _texture->getID()); 
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, static_cast<int>(_texture->getSize().getWidth()), static_cast<int>(_texture->getSize().getHeight()), 0);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Graphics::prepareForDrawing() const
	{
		glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
	}

	void Graphics::finishDrawing() const
	{
		glLoadIdentity();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);

		glBindTexture(GL_TEXTURE_2D, _texture->getID());
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		GLfloat screenVertices[] = { 0.0f, 0.0f,
									 0.0f, _texture->getSize().getHeight(),
									 _texture->getSize().getWidth(), _texture->getSize().getHeight(),
									 _texture->getSize().getWidth(), 0.0f };

		GLfloat textureVertices[] = { 0.0f, 0.0f,
									  0.0f, 1.0f,
								      1.0f, 1.0f,
									  1.0f, 0.0f };

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
		glVertexPointer(2, GL_FLOAT, 0, screenVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
 
		glDrawArrays(GL_QUADS, 0, 4);
 
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
 
			glDrawArrays(GL_LINE_LOOP, 0, 4);
 
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