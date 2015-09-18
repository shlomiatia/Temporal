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
		if (message.getID() == MessageID::TEMPORAL_PERIOD_CHANGED)
		{
			_isVisible = false;
		}
		else if (message.getID() == MessageID::SET_LIT)
		{
			_isLit = getBoolParam(message.getParam());
		}
		else if (message.getID() == MessageID::IS_VISIBLE)
		{
			_isVisible = _isLit && !_isCovered;
			message.setParam(&_isVisible);
		}
		else if (message.getID() == MessageID::IS_COVERED)
		{
			message.setParam(&_isCovered);
		}
		else if (message.getID() == MessageID::SENSOR_START)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if (params.getSensorId() == ACTIVATE_SENSOR_ID)
			{
				_isCovered = false;
			}
		}
		else if (message.getID() == MessageID::SENSOR_SENSE)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if (params.getSensorId() == ACTIVATE_SENSOR_ID && params.getContact().getTarget().getCategory() == CollisionCategory::COVER)
			{
				const OBB& body = params.getContact().getSource().getGlobalShape();
				const OBB& cover = params.getContact().getTarget().getGlobalShape();

				if (body.getLeft() >= cover.getLeft() && body.getRight() <= cover.getRight() && body.getBottom() >= cover.getBottom() && body.getTop() <= cover.getTop())
					_isCovered = true;
			}
		}
	}
}