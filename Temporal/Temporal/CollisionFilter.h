#ifndef COLLISIONFILTER_H
#define COLLISIONFILTER_H

#include "EntitySystem.h"

namespace Temporal
{
	class CollisionFilter : public Component
	{
	public:
		explicit CollisionFilter(int filter = -1, int group = -1);

		int getFilter() const { return _filter; }
		int getGroup() const { return _group; }

		ComponentType::Enum getType() const { return ComponentType::COLLISION_FILTER; }
		void handleMessage(Message& message);

		bool canCollide(int mask, int group) const;

	private:
		const int _filter;
		int _group;
	};
}

#endif