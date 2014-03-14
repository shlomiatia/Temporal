#include "Graphics.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "SceneNode.h"
#include "Shapes.h"
#include "Settings.h"
#include <gl/glew.h>
#include <SDL.h>

namespace Temporal
{
	void setColor(const Color& color)
	{
		glColor4f(color.getR(), color.getG(), color.getB(), color.getA());
	}

	static SDL_GLContext _context;

	void Graphics::init(const Settings& settings)
	{
		_resolution = settings.getResolution();
		float logicalViewWidth = settings.getViewY() * _resolution.getX() / _resolution.getY();
		_logicalView = Vector(logicalViewWidth, settings.getViewY());

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
		{
			abort();
			// ERROR: Error Failed initializing SDL video
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == -1)
		{
			// ERROR: Error Failed setting SDL OpenGL double buffering attribute
			abort();
		}

		
		int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (settings.isFullscreen()) flags |= SDL_WINDOW_FULLSCREEN;

		int resolutionX = static_cast<int>(settings.getResolution().getX());
		int resolutionY = static_cast<int>(settings.getResolution().getY());
		_window = SDL_CreateWindow("MyWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			resolutionX, resolutionY, flags);
		if (!_window) /* Die if creation failed */
			abort();
	
		glViewport(0, 0, resolutionX, resolutionY);

		_context = SDL_GL_CreateContext(_window);
		
		glewInit();
		glGetError(); // Stupid glewinit set error
		validate();


		if (!GLEW_VERSION_3_2) {
			abort();
		}

		SDL_GL_SetSwapInterval(1);
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		
		validate();
	}

	void Graphics::setTitle(const char* title) const
	{
		SDL_SetWindowTitle(_window, title);
	}

	void Graphics::dispose() const
	{
		SDL_GL_DeleteContext(_context);
		SDL_DestroyWindow(_window);
		SDL_Quit();
	}

	void Graphics::prepareForDrawing()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		_matrixStack.reset();
	}

	void Graphics::finishDrawing()
	{
		SDL_GL_SwapWindow(_window);
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

	void Graphics::bindTexture(unsigned int id)
	{
	}

	void Graphics::draw(const SceneNode& sceneNode, const SpriteSheet& spritesheet, const Color& color)
	{
		/*
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
		glPopMatrix();*/
	}

	void Graphics::draw(const Vector& position, const Texture& texture, const Color& color)
	{
	}

	void Graphics::draw(const Vector& position, const Texture& texture, const AABB& texturePart, const Color& color)
	{
	}

	void Graphics::draw(const AABB& rect, const Color& color, bool fill)
	{
	}

	void Graphics::draw(const AABB& rect, const Color& color1, const Color& color2)
	{
	}

	void Graphics::draw(const OBBAABBWrapper& obb, const Color& color, bool fill)
	{
	}

	void Graphics::draw(const OBB& obb, const Color& color, bool fill)
	{
	}

	void Graphics::draw(const Segment& segment, const Color& color)
	{
		
	}
}