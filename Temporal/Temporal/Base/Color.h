#pragma once
#include <assert.h>

namespace Temporal
{
	class Color
	{
	public:
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Black;
		static const Color White;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;

		Color(const float r, const float g, const float b)
			: _r(r), _g(g), _b(b)
		{
			assert(r >= 0.0f); assert(r <= 1.0f);
			assert(g >= 0.0f); assert(g <= 1.0f);
			assert(b >= 0.0f); assert(b <= 1.0f);
		}

		float getR(void) const { return _r; }
		float getG(void) const { return _g; }
		float getB(void) const { return _b; }
	private:
		const float _r;
		const float _g;
		const float _b;
	};
}