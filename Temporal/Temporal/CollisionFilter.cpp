#include "CollisionFilter.h"
#include "MessageUtils.h"

namespace Temporal
{
	CollisionFilter::CollisionFilter(int category, int group)
		: _category(category), _group(group) {}

	void CollisionFilter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLLISION_GROUP)
		{
			int group = getIntParam(message.getParam());
			_group = group;
		}
		else if(message.getID() == MessageID::GET_COLLISION_GROUP)
		{
			message.setParam(&_group);
		}
	}

	bool CollisionFilter::canCollide(int mask, int group) const
	{
		return (mask & _category) != 0 &&
			   (_group == -1 ||
			    group == -1 ||
			    _group == group);
	}
}