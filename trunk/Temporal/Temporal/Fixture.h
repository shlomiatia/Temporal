#ifndef FIXTURE_H
#define FIXTURE_H

#include "Shapes.h"
#include <stdio.h>

namespace Temporal
{
	class Hash;
	class Component;
	class CollisionFilter;
	class Transform;

	class Fixture
	{
	public:
		explicit Fixture(const Shape* shape = NULL) : _localShape(shape), _transform(NULL), _filter(NULL), _globalShape(NULL) {}
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
