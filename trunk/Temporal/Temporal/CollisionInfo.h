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
		CollisionInfo(const Transform& transform, const Shape* shape, int filter);

		const Shape& getLocalShape() const { return *_localShape; }
		const Shape& getGlobalShape() const;
		const int getFilter() const { return _filter; }
	private:
		const Transform& _transform;
		const Shape* _localShape;
		const int _filter;
		mutable Shape* _globalShape;
	};
}

#endif
