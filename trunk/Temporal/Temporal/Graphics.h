#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Color.h"
#include "Vector.h"

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;
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

		const Size& getResolution() const { return _resolution; }
		const Size& getLogicalView() const { return _logicalView; }

		void init(const Size& resolution, float logicalViewHeight, bool fullScreen = false);
		void dispose() const;

		void setTitle(const char* title) const;

		void prepareForDrawing() const;
		void finishDrawing() const;

		void validate() const;

		void translate(const Vector& translation) const;
		void draw(const SceneNode& sceneNode, const Color& color = Color::White);
		void draw(const Vector& position, const Texture& texture, const Color& color = Color::White);
		void draw(const Vector& position, const Texture& texture, const AABB& texturePart, const Color& color = Color::White);
		void draw(const AABB& rect, const Color& color = Color::White);
		void draw(const YABP& slopedArea, const Color& color = Color::White);
		void draw(const Segment& segment, const Color& color = Color::White);
		void draw(const Shape& shape, const Color& color = Color::White);

		void bindTexture(unsigned int id);

	private:
		static const int BIT_DEPTH = 32;

		unsigned int _lastTextureId;
		Size _resolution;
		Size _logicalView;

		Graphics() : _lastTextureId(0), _resolution(Size::Zero), _logicalView(Size::Zero) {}
		~Graphics() { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}


#endif