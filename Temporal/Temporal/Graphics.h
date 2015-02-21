#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Color.h"
#include "Vector.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "SpriteBatch.h"

class SDL_Window;

namespace Temporal
{
	class Settings;

	class Graphics
	{
	public:
		static Graphics& get()
		{
			static Graphics instance;
			return (instance);
		}

		const Vector& getResolution() const { return _resolution; }
		const Vector& getLogicalView() const { return _logicalView; }

		void init(const Settings& settings);
		void dispose() const;

		SDL_Window* getWindow() { return _window; }
		MatrixStack& getMatrixStack() { return _matrixStack; }
		ShaderProgram& getShaderProgram() { return _shaderProgram; }
		ShaderProgram& getFXShaderProgram() { return _fxShaderProgram; }
		SpriteBatch& getSpriteBatch() { return _spriteBatch; }
		SpriteBatch& getLinesSpriteBatch() { return _linesSpriteBatch; }
		SpriteBatch& getFXSpriteBatch() { return _fxSpriteBatch; }

		void setTitle(const char* title) const;

		void prepareForDrawing();
		void finishDrawing();

		void validate() const;

	private:
		static const int BIT_DEPTH = 32;
		SDL_Window* _window;

		Vector _resolution;
		Vector _logicalView;
		MatrixStack _matrixStack;
		ShaderProgram _shaderProgram;
		SpriteBatch _spriteBatch;
		SpriteBatch _linesSpriteBatch;
		ShaderProgram _fxShaderProgram;
		SpriteBatch _fxSpriteBatch;

		Graphics() : _resolution(Vector::Zero), _logicalView(Vector::Zero), _window(0), _linesSpriteBatch(_shaderProgram, SpriteBatchMode::LINES), _spriteBatch(_shaderProgram), _fxSpriteBatch(_fxShaderProgram) {}
		~Graphics() { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}


#endif