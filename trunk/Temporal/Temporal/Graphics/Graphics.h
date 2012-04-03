#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Temporal\Base\Color.h>
#include <Temporal\Base\NumericPair.h>

namespace Temporal
{
	class Rect;
	class Segment;
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

		void setTranslation(const Vector& translation) { _translation = translation; }

		void validate(void) const;

		void drawTexture(const Texture& texture, const Rect& texturePart, const Point& screenLocation, bool mirrored, const Color& color = Color::White) const;
		void drawRect(const Rect& rect, const Color& color = Color::White) const;
		void drawSegment(const Segment& segment, const Color& color = Color::White) const;

	private:
		static const int BIT_DEPTH = 32;
		Vector _translation;

		Graphics(void) : _translation(0.0f, 0.0f) {}
		~Graphics(void) { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}


#endif