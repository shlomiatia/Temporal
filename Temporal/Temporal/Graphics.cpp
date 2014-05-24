#include "Graphics.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "SceneNode.h"
#include "Shapes.h"
#include "Settings.h"
#include "Timer.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

		if (!GLEW_VERSION_3_2) {
			abort();
		}

		SDL_GL_SetSwapInterval(1);
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		
		validate();

		_shaderProgram.init("resources/shaders/v.glsl", "resources/shaders/f.glsl");
		glm::mat4 projection = glm::ortho(0.0f, _logicalView.getX(), 0.0f, _logicalView.getY(), -1.0f, 1.0f);
		_shaderProgram.setUniform(_shaderProgram.getUniform("u_projection"), glm::value_ptr(projection));
		_spriteBatch.init();
		_linesSpriteBatch.init();
		_shaderProgram.setUniform(_spriteBatch.getTypeUniform(), 0);

		_fxShaderProgram.init("resources/shaders/v.glsl", "resources/shaders/xf.glsl");
		projection = glm::ortho(0.0f, _resolution.getX(), _resolution.getY(), 0.0f, -1.0f, 1.0f);
		_fxShaderProgram.setUniform(_fxShaderProgram.getUniform("u_projection"), glm::value_ptr(projection));
		_fxSpriteBatch.init();
		_fxShaderProgram.setUniform(_fxSpriteBatch.getTypeUniform(), 0);
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
		_spriteBatch.begin();
		_linesSpriteBatch.begin();
	}

	void Graphics::finishDrawing()
	{
		_linesSpriteBatch.end();
		_spriteBatch.end();
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
}