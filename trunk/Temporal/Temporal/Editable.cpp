#include "Editable.h"
#include "MessageUtils.h"
#include "Mouse.h"
#include "ShapeOperations.h"

namespace Temporal
{
	const Hash Editable::TYPE = Hash("editable");

	void Editable::leftMouseDown(const MouseParams& params)
	{
		_offset = params.getPosition() - getPosition(*this);
		_translation = true;
		_rotation = false;
	}

	void Editable::rightMouseDown(const MouseParams& params)
	{
		_offset = params.getPosition() - getPosition(*this);
		_translation = false;
		_rotation = true;
	}

	void Editable::mouseMove(const MouseParams& params)
	{
		if(_translation)
		{
			Vector newPosition = params.getPosition() - _offset;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
		}
		else if(_rotation)
		{
			const Vector vector = params.getPosition() - _offset;
			//float rotation = fromRadians(vector.getAngle());
			//getCreateSceneNodeSample().setRotation(rotation);
		}
	}

	void Editable::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::MOUSE_DOWN)
		{
			MouseParams& params = getMouseParams(message.getParam());
			const OBB& shape = *static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE)));
			if(!intersects(shape, params.getPosition()))
				return;
			if(params.getButton() == MouseButton::LEFT)
			{
				leftMouseDown(params);
			}
			else
			{
				rightMouseDown(params);
			}
			
		}
		else if (message.getID() == MessageID::MOUSE_UP)
		{
			_translation = false;
			_rotation = false;
		}
		else if(message.getID() == MessageID::MOUSE_MOVE)
		{
			MouseParams& params = getMouseParams(message.getParam());
			mouseMove(params);
		}
	}
}