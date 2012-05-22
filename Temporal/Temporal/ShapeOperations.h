#ifndef SHAPEOPERATIONS_H
#define SHAPEOPERATIONS_H
#include <stdio.h>

namespace Temporal
{
	class Point;
	class Vector;
	class AABB;
	class Segment;
	class YABP;
	class Shape;
	class DirectedInterval;

	bool intersects(const AABB& rect1, const AABB& rect2, Vector* correction);
	bool intersects(const AABB& rect, const Segment& seg, Vector* correction);
	bool intersects(const DirectedInterval& dir, const AABB& rect, Point* pointOfIntersection);
	bool intersects(const DirectedInterval& dir, const Segment& seg, Point* pointOfIntersection);
	bool intersects(const YABP& yabp, const AABB& aabb);
	bool intersects(const YABP& yabp1, const YABP& yabp2);
	bool intersects(const YABP& yabp, const Segment& segment);

	bool intersects(const AABB& rect, const Shape& shape, Vector* correction = NULL);
	bool intersects(const DirectedInterval& dir, const Shape& shape, Point* pointOfIntersection = NULL);
	bool intersects(const YABP& yabp, const Shape& shape);
}

#endif