#ifndef COLLISIONINFO_H
#define COLLISIONINFO_H

#include "CollisionFilter.h"

namespace Temporal
{
	class Hash;
	class Transform;
	class Shape;

	class CollisionInfo
	{
	public:
		CollisionInfo(const Transform& transform, const CollisionFilter& filter, const Shape* shape);
		~CollisionInfo() { delete _globalShape; }

		const Shape& getLocalShape() const { return *_localShape; }
		const Shape& getGlobalShape() const { return *_globalShape; }
		const CollisionFilter& getFilter() { return _filter; }
		const Hash& getEntityId();

		void update();
	private:
		const Transform& _transform;
		const CollisionFilter& _filter;
		const Shape* _localShape; 
		Shape* _globalShape;
	};
}

#endif
