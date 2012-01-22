#include "Color.h"
#include <assert.h>

namespace Temporal
{
	Color::Color(float r, float g, float b)
		: _r(r), _g(g), _b(b)
		{
			assert(r >= 0.0f); assert(r <= 1.0f);
			assert(g >= 0.0f); assert(g <= 1.0f);
			assert(b >= 0.0f); assert(b <= 1.0f);
		}

	const Color Color::Red(1.0f, 0.0f, 0.0f);
	const Color Color::Green(0.0f, 1.0f, 0.0f);
	const Color Color::Blue(0.0f, 0.0f, 1.0f);
	const Color Color::Black(0.0f, 0.0f, 0.0f);
	const Color Color::White(1.0f, 1.0f, 1.0f);
	const Color Color::Yellow(1.0f, 1.0f, 0.0f);
	const Color Color::Magenta(1.0f, 0.0f, 1.0f);
	const Color Color::Cyan(0.0f, 1.0f, 1.0f);
}