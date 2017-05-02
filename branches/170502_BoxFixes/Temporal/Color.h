#ifndef COLOR_H
#define COLOR_H

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
		static const Color Transparent;

		Color(float r, float g, float b, float a = 1.0f);

		float getR() const { return _r; }
		void setR(float r) { _r = r; }
		float getG() const { return _g; }
		void setG(float g) { _g = g; }
		float getB() const { return _b; }
		void setB(float b) { _b = b; }
		float getA() const { return _a; }
		void setA(float a) { _a = a; }

		bool operator==(const Color& color) const { return getR() == color.getR() && getG() == color.getG() && getB() == color.getB() && getA() == color.getA(); }
		bool operator!=(const Color& color) const { return !(*this == color); }
		
	private:
		float _r;
		float _g;
		float _b;
		float _a;

		friend class SerializationAccess;
	};
}
#endif