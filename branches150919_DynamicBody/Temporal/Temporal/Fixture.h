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
		explicit Fixture(const OBB& shape = OBB::Zero) : _localShape(shape), _transform(0), _filter(0) {}

		void init(const Component& parent);

		const OBB& getLocalShape() const { return _localShape; }
		OBB& getLocalShape() { return _localShape; }
		const OBB& getGlobalShape() const { return _globalShape; }
		OBB& getGlobalShape() { return _globalShape; }
		int getCategory() const;
		int getGroup() const;
		bool canCollide(int mask, int group) const;
		Hash getEntityId() const;

		void update();

		Fixture* clone() const;
	private:
		const Transform* _transform;
		const CollisionFilter* _filter;
		OBB _localShape; 
		OBB _globalShape;

		friend class SerializationAccess;

		Fixture(const Fixture&);
		Fixture& operator=(const Fixture&);
	};
}

#endif
