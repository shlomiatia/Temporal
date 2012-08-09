#ifndef COLLISIONFILTER_H
#define COLLISIONFILTER_H

#include "EntitySystem.h"

namespace Temporal
{
	class CollisionFilter : public Component
	{
	public:
		explicit CollisionFilter(int filter = -1, int mask = -1, int group = -1);

		int getFilter() const { return _filter; }
		int getMask() const { return _mask; }
		int getGroup() const { return _group; }

		ComponentType::Enum getType() const { return ComponentType::COLLISION_FILTER; }
		void handleMessage(Message& message);

		bool canCollide(int mask, int group) const;

	private:
		const int _filter;
		const int _mask;
		int _group;
	};
}

#endif