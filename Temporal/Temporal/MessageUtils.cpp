#include "MessageUtils.h"
#include "EntitySystem.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	const Vector& getPosition(const Component& component)
	{
		return *static_cast<Vector*>(component.raiseMessage(Message(MessageID::GET_POSITION)));
	}

	Side::Enum getOrientation(const Component& component)
	{
		return *static_cast<Side::Enum*>(component.raiseMessage(Message(MessageID::GET_ORIENTATION)));
	}

	Hash getHashParam(void* data)
	{
		return *static_cast<Hash*>(data);
	}

	const Vector& getVectorParam(void* data)
	{
		return *static_cast<const Vector*>(data);
	}

	void sendDirectionAction(const Component& component, Side::Enum direction)
	{
		Side::Enum orientation = getOrientation(component);
		if(direction == orientation)
			component.raiseMessage(Message(MessageID::ACTION_FORWARD));
		else
			component.raiseMessage(Message(MessageID::ACTION_BACKWARD));
	}
}