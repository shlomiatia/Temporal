#pragma once
#include "Vector.h"

namespace Temporal
{
	class Line
	{
	public:

		Line(const float x1, const float y1, const float x2, const float y2)
			: _point1(x1, y1), _point2(x2, y2) {}

		Line(const Vector& point1, const Vector& point2)
			: _point1(point1), _point2(point2) {}

		const Vector& getPoint1(void) const { return _point1; }
		void setPoint1(const Vector& point1) { _point1 = point1; }
		const Vector& getPoint2(void) const { return _point2;	}
		void setPoint2(const Vector& point2) { _point2 = point2; }

		float getX1(void) const { return _point1.getX(); }
		void setX1(float x1) { _point1.setX(x1); }
		float getY1(void) const { return _point1.getY(); }
		void setY1(float y1) { _point1.setY(y1); }
		float getX2(void) const { return _point2.getX(); }
		void setX2(float x2) { _point2.setX(x2); }
		float getY2(void) const { return _point2.getY(); }
		void setY2(float y2) { _point2.setY(y2); }
	private:
		Vector _point1;
		Vector _point2;
	};
}