#include "ActivationSensor.h"
#include "MessageUtils.h"

namespace Temporal
{
	void ActivationSensor::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::SENSOR_SENSE)
		{
			_shouldActivate = true;
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			if (_target != Hash::INVALID)
			{
				bool isActivated = getBoolParam(getEntity().getManager().sendMessageToEntity(_target, Message(MessageID::IS_ACTIVATED)));
				if (_shouldActivate != isActivated)
				{
					getEntity().getManager().sendMessageToEntity(_target, Message(MessageID::TOGGLE_ACTIVATION));
				}
				
			}
			_shouldActivate = false;
		}
	
	}
}