#ifndef SHAPEOPERATIONS_H
#define SHAPEOPERATIONS_H
#include <stdio.h>

namespace Temporal
{
	class Vector;
	class Vector;
	class AABB;
	class Segment;
	class YABP;
	class Shape;
	class DirectedSegment;

	bool intersects(const AABB& rect1, const AABB& rect2, Vector* correction);
	bool intersects(const AABB& rect, const Segment& seg, Vector* correction);
	bool intersects(const DirectedSegment& seg, const AABB& rect, Vector* pointOfIntersection, float* distance);
	bool intersects(const DirectedSegment& dirSeg, const Segment& seg, Vector* pointOfIntersection, float* distance);
	bool intersects(const YABP& yabp, const AABB& aabb);
	bool intersects(const YABP& yabp1, const YABP& yabp2);
	bool intersects(const YABP& yabp, const Segment& segment);

	bool intersects(const Shape& shape1, const Shape& shape2, Vector* correction = NULL);
	bool intersects(const DirectedSegment& seg, const Shape& shape, Vector* pointOfIntersection = NULL, float* distance = NULL);
	bool intersects(const YABP& yabp, const Shape& shape);
}

#endif