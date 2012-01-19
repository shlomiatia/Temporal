#pragma once
#include <Temporal\Base\Base.h>
#include "Texture.h"

namespace Temporal
{
	class Graphics
	{
	public:
		// gets the singleton Graphics instance
		static Graphics& get(void)
		{
			static Graphics instance;
			return (instance);
		}

		// create a window (replacing existing window, if any) as the screen view surface
		void init(const Vector& resolution, const Vector& viewSize, const bool fullScreen = false);

		// destroys the Graphics, disposing of all resources and closing the window
		void dispose(void) const;

		// set the Graphics window properties
		void setVideoMode(const Vector& resolution, const Vector& viewSize, const bool fullScreen = false) const;

		void setTitle(const char* title) const;

		// Handle stuff that need to be done before drawing
		void prepareForDrawing(void) const;

		// Handle stuff that need to be done after drawing
		void finishDrawing(void) const;

		Vector setTranslation(const Vector& translation) { _translation = translation; }

		void validate(void) const;

		// draws shapes to the screen to the screen
		void drawTexture(const Texture& texture, const Rect& texturePart, const Vector& screenLocation, bool mirrored) const;
		void drawRect(const Rect& rect, const Color& color = Color::White) const;

	private:
		static const int BIT_DEPTH = 32;
		Vector _translation;

		Graphics(void) : _translation(0.0f, 0.0f) {}
		~Graphics(void) { dispose(); }
		Graphics(const Graphics&);
		Graphics& operator=(const Graphics&);
	};
}

