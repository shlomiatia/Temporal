#include "PatrolWalk.h"
#include "Patrol.h"
#include "ActionController.h"

namespace Temporal
{
	static const Hash WAIT_STATE = Hash("PAT_STT_WAIT");

	static const Hash PATROL_CONTROL_SENSOR_ID = Hash("SNS_PATROL_CONTROL");

	namespace PatrolStates
	{
		void Walk::handleMessage(Message& message)
		{
			Patrol& patrol = *static_cast<Patrol*>(_stateMachine);

			if (patrol.handleTakedownMessage(message) ||
				patrol.handleFireMessage(message) ||
				patrol.handleAlarmMessage(message))
			{
			}
			else if (message.getID() == MessageID::COLLISIONS_CORRECTED)
			{
				const Vector& collision = getVectorParam(message.getParam());
				if (collision.getX() != 0.0f && collision.getY() >= 0.0f)
				{
					_stateMachine->changeState(WAIT_STATE);
				}
			}
			else if (message.getID() == MessageID::SENSOR_FRONG_EDGE)
			{
				_stateMachine->changeState(WAIT_STATE);
			}
			else if (message.getID() == MessageID::SENSOR_SENSE)
			{
				const SensorParams& params = getSensorParams(message.getParam());
				if (params.getSensorId() == PATROL_CONTROL_SENSOR_ID)
				{
					_stateMachine->changeState(WAIT_STATE);
				}
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			}
		}
	}
}
