#ifndef SHAPEOPERATIONS_H
#define SHAPEOPERATIONS_H

namespace Temporal
{
	class Vector;
	class YABP;
	class DirectedSegment;

	bool intersects(const YABP& yabp1, const YABP& yabp2, Vector* correction = 0);
	bool intersects(const DirectedSegment& seg, const YABP& yabp, Vector* pointOfIntersection = 0, float* distance = 0);
	bool intersects(const YABP& yabp, const Vector& point);
}

#endif