#include "TemporalNotification.h"

namespace Temporal
{
	void TemporalNotification::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_READY)
		{
			raiseMessage(Message(MessageID::START_EMITTER));
		}
		else if (message.getID() == MessageID::EMITTER_FINISHED)
		{
			getEntity().getManager().remove(getEntity().getId());
		}
	}
}