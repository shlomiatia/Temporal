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
		static Graphics& get()
		{
			static Graphics instance;
			return (instance);
		}

		void init(const Size& resolution, const Size& viewSize, bool fullScreen = false);
		void dispose() const;

		void setVideoMode(const Size& resolution, const Size& viewSize, bool fullScreen = false) const;
		void setTitle(const char* title) const;

		void prepareForDrawing() const;
		void finishDrawing() const;

		void validate() const;

		void translate(const Vector& translation) const;
		void draw(const SpriteSheet& spritesheet, const SceneNode& sceneNode, const Color& color = Color::White) const;
		void draw(const Point& position, const Texture& texture, const Color& color = Color::White) const;
		void draw(const Point& position, const Texture& texture, const AABB& texturePart, const Color& color = Color::White) const;
		void draw(const AABB& rect, const Color& color = Color::White) const;
		void draw(const YABP& slopedArea, const Color& color = Color::White) const;
		void draw(const Segment& segment, const Color& color = Color::White) const;
		void draw(const Shape& shape, const Color& color = Color::White) const;

		void bindTexture(unsigned int id) const;

	private:
		static const int BIT_DEPTH = 32;

		mutable unsigned int _lastTextureId;

		Graphics() : _lastTextureId(0) {}
		~Graphics() { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}


#endif