#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Color.h"
#include "Vector.h"
#include "MatrixStack.h"

class SDL_Window;

namespace Temporal
{
	class Settings;
	class SceneNode;
	class SpriteSheet;
	class AABB;
	class Segment;
	class OBB;
	class OBBAABBWrapper;
	class Texture;

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

		void setTitle(const char* title) const;

		void prepareForDrawing();
		void finishDrawing();

		void validate() const;

		void draw(const SceneNode& sceneNode, const SpriteSheet& spritesheet, const Color& color = Color::White);
		void draw(const Vector& position, const Texture& texture, const Color& color = Color::White);
		void draw(const Vector& position, const Texture& texture, const AABB& texturePart, const Color& color = Color::White);
		void draw(const AABB& rect, const Color& color = Color::White, bool fill = false);
		void draw(const AABB& rect, const Color& color1, const Color& color2);
		void draw(const OBB& obb, const Color& color = Color::White, bool fill = false);
		void draw(const OBBAABBWrapper& obb, const Color& color = Color::White, bool fill = false);
		void draw(const Segment& segment, const Color& color = Color::White);

		void bindTexture(unsigned int id);

	private:
		static const int BIT_DEPTH = 32;
		SDL_Window* _window;

		MatrixStack _matrixStack;
		Vector _resolution;
		Vector _logicalView;

		Graphics() : _resolution(Vector::Zero), _logicalView(Vector::Zero), _window(0) {}
		~Graphics() { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}


#endif