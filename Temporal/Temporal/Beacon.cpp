#include "Beacon.h"
#include "MessageUtils.h"

namespace Temporal
{
	void Beacon::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::GAME_STATE_READY)
		{
			Hash id = getEntity().getId();
			
			SourceTargetParams params(id, id);
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::ALARM, &params));
		}
	}
}