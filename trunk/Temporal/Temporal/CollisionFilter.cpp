#include "CollisionFilter.h"

namespace Temporal
{
	CollisionFilter::CollisionFilter(int filter, int mask, int group)
		: _filter(filter), _mask(mask), _group(group) {}

	void CollisionFilter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLLISION_GROUP)
		{
			int group = *static_cast<int*>(message.getParam());
			_group = group;
		}
	}

	bool CollisionFilter::canCollide(int mask, int group) const
	{
		return (mask & _filter) != 0 &&
			   (_group == -1 ||
			    group == -1 ||
			    _group == group);
	}
}