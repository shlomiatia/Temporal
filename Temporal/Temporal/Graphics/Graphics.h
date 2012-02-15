#pragma once
#include <Temporal\Base\Base.h>
#include "Texture.h"

namespace Temporal
{
	class Graphics
	{
	public:
		static Graphics& get(void)
		{
			static Graphics instance;
			return (instance);
		}

		void init(const Vector& resolution, const Vector& viewSize, bool fullScreen = false);
		void dispose(void) const;

		void setVideoMode(const Vector& resolution, const Vector& viewSize, bool fullScreen = false) const;
		void setTitle(const char* title) const;

		void prepareForDrawing(void) const;
		void finishDrawing(void) const;

		void setTranslation(const Vector& translation) { _translation = translation; }

		void validate(void) const;

		void drawTexture(const Texture& texture, const Rect& texturePart, const Vector& screenLocation, bool mirrored, const Color& color = Color::White) const;
		void drawRect(const Rect& rect, const Color& color = Color::White) const;
		void drawLine(const Vector& p1, const Vector& p2, const Color& color = Color::White) const;

	private:
		static const int BIT_DEPTH = 32;
		Vector _translation;

		Graphics(void) : _translation(0.0f, 0.0f) {}
		~Graphics(void) { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}

