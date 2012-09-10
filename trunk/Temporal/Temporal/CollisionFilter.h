#ifndef COLLISIONFILTER_H
#define COLLISIONFILTER_H

#include "EntitySystem.h"

namespace Temporal
{
	class CollisionFilter : public Component
	{
	public:
		explicit CollisionFilter(int category = -1, int group = -1);

		int getCategory() const { return _category; }
		int getGroup() const { return _group; }

		ComponentType::Enum getType() const { return ComponentType::COLLISION_FILTER; }
		void handleMessage(Message& message);

		bool canCollide(int mask, int group) const;

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("category", _category);
			serializer.serialize("group", _group);
		}

	private:
		int _category;
		int _group;
	};
}

#endif