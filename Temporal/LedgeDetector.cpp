#include "LedgeDetector.h"
#include "MessageUtils.h"
#include "Math.h"
#include "ShapeOperations.h"

namespace Temporal
{
	void LedgeDetector::start()
	{
		_up = 0;
		_upFailed = false;
		_down = 0;
		_downFailed = false;
		_front = 0;
		_frontFailed = false;
		_height = 0.0f;

		OBB& body = *static_cast<OBB*>(getOwner().raiseMessage(Message(MessageID::GET_SHAPE)));
		_body.setOBB(body);
		_max = _body.getTop() + _body.getHeight();
		_side = getOrientation(getOwner());
	}

	void LedgeDetector::handleUp(const Contact& contact)
	{
		if (_upFailed)
			return;
		const OBB& sensor = contact.getSource().getGlobalShape();
		const OBB& platform = contact.getTarget().getGlobalShape();
		if (platform.getAxisX().getY() == 0.0f &&
			equals(_body.getTop(), platform.getCenterY()))
		{
			_up = const_cast<Fixture*>(&contact.getTarget());
		}
		else
		{
			OBB test = OBBAABB(_body.getCenter() + Vector(0.0f, _body.getHeight()), _body.getRadius());

			if (intersectsExclusive(test, platform))
			{
				_upFailed = true;
				_up = 0;
			}
		}
	}

	void LedgeDetector::handleDown(const Contact& contact)
	{
		if (_downFailed)
			return;
		const OBB& sensor = contact.getSource().getGlobalShape();
		const OBB& platform = contact.getTarget().getGlobalShape();
		if (platform.getAxisX().getY() == 0.0f &&
			equals(_body.getBottom(), platform.getCenterY()))
		{
			_down = const_cast<Fixture*>(&contact.getTarget());;
		}
		else
		{
			OBB test = OBBAABB(_body.getCenter() - Vector(0.0f, _body.getHeight()), _body.getRadius());
			Vector correction = Vector::Zero;
			if (intersectsExclusive(test, platform))
			{
				_downFailed = true;
				_down = 0;
			}
		}
	}

	void LedgeDetector::handleFrontCheckY(float y)
	{
		if (y > _body.getTop())
		{
			_max = std::fminf(_max, y);
		}
		// Blocks from below
		else if (y > _body.getBottom())
		{
			_front = 0;
			_frontFailed = true;
		}
	}

	void LedgeDetector::handleFront(const Contact& contact)
	{
		if (_frontFailed)
			return;
		const OBB& platform = contact.getTarget().getGlobalShape();

		Side::Enum opposite = Side::getOpposite(_side);
		if (platform.getAxisX().getY() == 0.0f &&
			equals(_body.getSide(_side), platform.getSide(opposite)) &&
			_body.getTop() >= platform.getTop() &&
			_body.getBottom() <= platform.getTop())
		{
			float height = platform.getTop() - _body.getBottom();
			if (height > _height)
			{
				_height = height;
				_front = const_cast<Fixture*>(&contact.getTarget());
			}
		}
		else
		{
			float x = _body.getSide(_side);
			if (x >= platform.getLeft() && x <= platform.getRight())
			{
				handleFrontCheckY(platform.getTopSegment(x).getY(x));
				handleFrontCheckY(platform.getBottomSegment(x).getY(x));
			}

		}
	}

	void LedgeDetector::handle(const Contact& contact)
	{
		handleUp(contact);
		handleDown(contact);
		handleFront(contact);
	}

	void LedgeDetector::end()
	{
		if (_up)
			getOwner().raiseMessage(Message(MessageID::SENSOR_HANG_UP, _up));
		if (_down)
			getOwner().raiseMessage(Message(MessageID::SENSOR_DESCEND, _down));

		// Blocks from above
		if (_body.getTop() + _height > _max)
		{
			_front = 0;
			_frontFailed = true;
		}
		if (_front)
			getOwner().raiseMessage(Message(MessageID::SENSOR_HANG_FRONT, _front));
	}
}
