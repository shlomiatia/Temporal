#ifndef COLLISIONFILTER_H
#define COLLISIONFILTER_H

#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class CollisionFilter : public Component
	{
	public:
		explicit CollisionFilter(int category = -1, int group = -1);

		int getCategory() const { return _category; }
		int getGroup() const { return _group; }

		Hash getType() const { return ComponentsIds::COLLISION_FILTER; }
		void handleMessage(Message& message);

		Component* clone() const { return new CollisionFilter(_category, _group); }
		
	private:

		int _category;
		int _group;

		friend class SerializationAccess;
	};
}

#endif