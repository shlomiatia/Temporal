#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Color.h"
#include "NumericPair.h"

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;
	class Shape;
	class AABB;
	class Segment;
	class YABP;
	class Texture;

	class Graphics
	{
	public:
		static Graphics& get(void)
		{
			static Graphics instance;
			return (instance);
		}

		void init(const Size& resolution, const Size& viewSize, bool fullScreen = false);
		void dispose(void) const;

		void setVideoMode(const Size& resolution, const Size& viewSize, bool fullScreen = false) const;
		void setTitle(const char* title) const;

		void prepareForDrawing(void) const;
		void finishDrawing(void) const;

		void validate(void) const;

		void translate(const Vector& translation) const;
		void draw(const SceneNode& sceneNode, const SpriteSheet& spritesheet, Side::Enum orientation, const Color& color = Color::White) const;
		void draw(const AABB& rect, const Color& color = Color::White) const;
		void draw(const YABP& slopedArea, const Color& color = Color::White) const;
		void draw(const Segment& segment, const Color& color = Color::White) const;
		void draw(const Shape& shape, const Color& color = Color::White) const;

	private:
		static const int BIT_DEPTH = 32;

		Graphics(void) {}
		~Graphics(void) { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}


#endif