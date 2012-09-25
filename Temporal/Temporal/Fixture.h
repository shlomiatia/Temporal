#ifndef FIXTURE_H
#define FIXTURE_H

#include "Shapes.h"

namespace Temporal
{
	class Hash;
	class Component;
	class CollisionFilter;
	class Transform;

	class Fixture
	{
	public:
		explicit Fixture(const Shape* shape = 0) : _localShape(shape), _transform(0), _filter(0), _globalShape(0) {}
		~Fixture() { delete _localShape; delete _globalShape; }

		void init(const Component& parent);

		const Shape& getLocalShape() const { return *_localShape; }
		const Shape& getGlobalShape() const { return *_globalShape; }
		Shape& getGlobalShape() { return *_globalShape; }
		const CollisionFilter& getFilter() const { return *_filter; }
		Hash getEntityId() const;

		void update();

		Fixture* clone() const;
	private:
		const Transform* _transform;
		const CollisionFilter* _filter;
		const Shape* _localShape; 
		Shape* _globalShape;
	};
}

#endif
