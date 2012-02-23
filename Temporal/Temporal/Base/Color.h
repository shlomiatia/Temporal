#pragma once

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

		inline Color(float r, float g, float b, float a = 1.0f);

		float getR(void) const { return _r; }
		float getG(void) const { return _g; }
		float getB(void) const { return _b; }
		float getA(void) const { return _a; }
		
	private:
		float _r;
		float _g;
		float _b;
		float _a;
	};
}