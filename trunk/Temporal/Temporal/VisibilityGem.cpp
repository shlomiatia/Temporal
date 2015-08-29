#include "VisibilityGem.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "Sensor.h"

namespace Temporal
{
	const Hash VisibilityGem::TYPE = Hash("visibility-gem");
	static const Hash ACTIVATE_SENSOR_ID = Hash("SNS_ACTIVATE");

	void VisibilityGem::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT) 
		{
			_isVisible = false;
		}
		if (message.getID() == MessageID::SET_LIT)
		{
			_isLit = getBoolParam(message.getParam());
		}
		else if (message.getID() == MessageID::SENSOR_SENSE)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if (params.getSensorId() == ACTIVATE_SENSOR_ID && params.getContact().getTarget().getCategory() == CollisionCategory::COVER)
			{
				const OBB& body = params.getContact().getSource().getGlobalShape();
				const OBB& cover = params.getContact().getTarget().getGlobalShape();
				
				if (body.getLeft() >= cover.getLeft() && body.getRight() <= cover.getRight() && body.getBottom() >= cover.getBottom() && body.getTop() <= cover.getTop())
					_isVisible = false;
			}
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			_isVisible = true;
		}
		else if (message.getID() == MessageID::IS_VISIBLE)
		{
			_isVisible &= _isLit;
			message.setParam(&_isVisible);
		}
	}
}