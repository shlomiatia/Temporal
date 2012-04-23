#ifndef SHAPEOPERATIONS_H
#define SHAPEOPERATIONS_H
#include <stdio.h>

namespace Temporal
{
	class Point;
	class Vector;
	class AABB;
	class Segment;
	class Shape;
	class DirectedSegment;

	bool intersects(const AABB& rect1, const AABB& rect2, Vector* correction);
	bool intersects(const AABB& rect, const Segment& seg, Vector* correction);
	bool intersects(const DirectedSegment& seg, const AABB& rect, Point* pointOfIntersection);
	bool intersects(const DirectedSegment& dirSeg, const Segment& seg, Point* pointOfIntersection);

	bool intersects(const Shape& shape1, const Shape& shape2, Vector* correction = NULL);
	bool intersects(const DirectedSegment& seg, const Shape& shape, Point* pointOfIntersection = NULL);
}

#endif