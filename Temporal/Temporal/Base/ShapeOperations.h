#ifndef SHAPEOPERATIONS_H
#define SHAPEOPERATIONS_H
#include <stdio.h>

namespace Temporal
{
	class Point;
	class Vector;
	class Rectangle;
	class Segment;
	class Shape;
	class DirectedSegment;

	bool intersects(const Rectangle& rect1, const Rectangle& rect2, Vector* correction);
	bool intersects(const Rectangle& rect, const Segment& seg, Vector* correction);
	bool intersects(const DirectedSegment& seg, const Rectangle& rect, Point* pointOfIntersection);
	bool intersects(const DirectedSegment& dirSeg, const Segment& seg, Point* pointOfIntersection);

	bool intersects(const Shape& shape1, const Shape& shape2, Vector* correction = NULL);
	bool intersects(const DirectedSegment& seg, const Shape& shape, Point* pointOfIntersection = NULL);
}

#endif