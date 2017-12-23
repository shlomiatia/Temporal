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
		else if(message.getID() == MessageID::SET_COLLISION_CATEGORY)
		{
			int category = getIntParam(message.getParam());
			_category = category;
		}
		else if (message.getID() == MessageID::GET_COLLISION_CATEGORY)
		{
			message.setParam(&_category);
		}
		
	}
}