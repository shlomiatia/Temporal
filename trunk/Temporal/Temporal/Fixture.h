#ifndef FIXTURE_H
#define FIXTURE_H

#include "CollisionFilter.h"

namespace Temporal
{
	class Hash;
	class Transform;
	class Shape;

	class Fixture
	{
	public:
		Fixture(const Transform& transform, const CollisionFilter& filter, const Shape* shape);
		~Fixture() { delete _globalShape; }

		const Shape& getLocalShape() const { return *_localShape; }
		const Shape& getGlobalShape() const { return *_globalShape; }
		const CollisionFilter& getFilter() const { return _filter; }
		const Hash& getEntityId() const;

		void update();
	private:
		const Transform& _transform;
		const CollisionFilter& _filter;
		const Shape* _localShape; 
		Shape* _globalShape;
	};
}

#endif
