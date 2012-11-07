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
		explicit Fixture(const YABP& shape = YABP::Zero) : _localShape(shape), _transform(0), _filter(0) {}

		void init(const Component& parent);

		const YABP& getLocalShape() const { return _localShape; }
		const YABP& getGlobalShape() const { return _globalShape; }
		YABP& getGlobalShape() { return _globalShape; }
		const CollisionFilter& getFilter() const { return *_filter; }
		Hash getEntityId() const;

		void update();

		Fixture* clone() const;
	private:
		const Transform* _transform;
		const CollisionFilter* _filter;
		YABP _localShape; 
		YABP _globalShape;

		friend class SerializationAccess;
	};
}

#endif
