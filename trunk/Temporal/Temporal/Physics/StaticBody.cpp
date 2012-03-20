#include "StaticBody.h"
#include <Temporal\Game\Message.h>

namespace Temporal
{
	void StaticBody::handleMessage(Message& message)
	{
		Body::handleMessage(message);
		if(message.getID() == MessageID::IS_COVER)
		{
			message.setParam(&_cover);
		}
	}
}