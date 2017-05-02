#ifndef SHAPEOPERATIONS_H
#define SHAPEOPERATIONS_H

namespace Temporal
{
	class Vector;
	class DirectedSegment;
	class OBB;

	bool intersects(const OBB& obb, const Vector& point);
	bool intersects(const DirectedSegment& seg, const OBB& obb, Vector* pointOfIntersection = 0, float* distance = 0);
	bool intersects(const OBB& obb1, const OBB& obb2, Vector* correction = 0);
	
	bool intersectsExclusive(const OBB& o1, const OBB& o2, Vector* correction = 0);
}

#endif