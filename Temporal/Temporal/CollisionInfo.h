#ifndef COLLISIONINFO_H
#define COLLISIONINFO_H

#include "Hash.h"
#include "CollisionFilter.h"

namespace Temporal
{
	class Transform;
	class Shape;

	class CollisionInfo
	{
	public:
		CollisionInfo(const Transform& transform, const CollisionFilter& filter, const Shape* shape);

		const Shape& getLocalShape() const { return *_localShape; }
		const Shape& getGlobalShape();
		const CollisionFilter& getFilter() { return _filter; }
	private:
		const Transform& _transform;
		const CollisionFilter& _filter;
		const Shape* _localShape; 
		Shape* _globalShape;
	};
}

#endif
