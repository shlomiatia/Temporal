#ifndef COLLISIONINFO_H
#define COLLISIONINFO_H

#include "Hash.h"

namespace Temporal
{
	class Transform;
	class Shape;

	class CollisionInfo
	{
	public:
		CollisionInfo(const Transform& transform, const Shape* shape, int filter1 = -1, int mask1 = -1, int filter2 = -1, int mask2 = -1);

		const Shape& getLocalShape() const { return *_localShape; }
		const Shape& getGlobalShape();

		int getFilter1() const { return _filter1; }
		int getMask1() const { return _mask1; }
		int getFilter2() const { return _filter2; }
		int getMask2() const { return _mask2; }

		bool canCollide(const CollisionInfo& other) const;
		bool canCollide(int mask1, int mask2) const {  return (mask1 & _filter1) != 0 && (mask2 & _filter2) != 0; }
	private:
		const Transform& _transform;
		const Shape* _localShape; 
		Shape* _globalShape;
		const int _filter1;
		const int _mask1;
		const int _filter2;
		const int _mask2;
	};
}

#endif
