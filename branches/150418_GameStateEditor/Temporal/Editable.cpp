#include "Editable.h"
#include "MessageUtils.h"
#include "Mouse.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "Layer.h"
#include "StaticBody.h"
#include "Fixture.h"
#include "Grid.h"

namespace Temporal
{
	static Hash STATIC_BODY_TYPE("static-body");
	const Hash Editable::TYPE("editable");

	Hash Editable::getType() const
	{
		return TYPE;
	}

	void Editable::reset()
	{
		_translation = false;
		_rotation = false;
		_scale = false;
	}

	void Editable::mouseDown(MouseParams& params)
	{
		const OBB& shape = *static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE)));
		if(intersects(shape, params.getPosition()))
		{
			if(params.getButton() == MouseButton::LEFT)
			{
				reset();
				_translation = true;
				_translationOffset = params.getPosition() - getPosition(*this);
				params.setHandled(true);
			}
			else
			{
				reset();
				_rotation = true;
				params.setHandled(true);
			}
		}
		else if(params.getButton() == MouseButton::LEFT)
		{
			if(intersects(_positiveXScale, params.getPosition()))
			{
				reset();
				_scale =  true;
				_isPositiveScale = true;
				_scaleAxis = Axis::X;
				params.setHandled(true);
			}
			else if(intersects(_negativeXScale, params.getPosition()))
			{
				reset();
				_scale =  true;
				_isPositiveScale = false;
				_scaleAxis = Axis::X;
				params.setHandled(true);
			}
			else if(intersects(_positiveYScale, params.getPosition()))
			{
				reset();
				_scale =  true;
				_isPositiveScale = true;
				_scaleAxis = Axis::Y;
				params.setHandled(true);
			}
			else if(intersects(_negativeYScale, params.getPosition()))
			{
				reset();
				_scale =  true;
				_isPositiveScale = false;
				_scaleAxis = Axis::Y;
				params.setHandled(true);
			}
			
		}
	}

	void Editable::mouseMove(MouseParams& params)
	{
		if(_translation)
		{
			Vector newPosition = params.getPosition() - _translationOffset;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			params.setHandled(true);
		}
		else if(_rotation)
		{
			Vector vector = (params.getPosition() - getPosition(*this)).normalize();
			StaticBody& staticBody = *static_cast<StaticBody*>(getEntity().get(STATIC_BODY_TYPE));
			staticBody.getFixture().getLocalShape().setAxis0(vector);
			getEntity().getManager().getGameState().getGrid().update(&staticBody.getFixture());
			params.setHandled(true);
		}
		else if(_scale)
		{
			Vector position = getPosition(*this);
			Vector vector = params.getPosition() - position;
			const OBB& shape = *static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE)));
			Vector axis = shape.getAxis(_scaleAxis);
			if(!_isPositiveScale)
				axis = -axis;
			Vector radius = shape.getRadius();
			float axisRadius = radius.getAxis(_scaleAxis);
			float delta = (vector * axis - axisRadius) / 2.0f;
			radius.setAxis(_scaleAxis, axisRadius + delta);
			if(radius.getAxis(_scaleAxis) < 0.0f) return;
			StaticBody& staticBody = *static_cast<StaticBody*>(getEntity().get(STATIC_BODY_TYPE));
			staticBody.getFixture().getLocalShape().setRadius(radius);
			getEntity().getManager().getGameState().getGrid().update(&staticBody.getFixture());
			Vector newPosition = position + axis * delta;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			params.setHandled(true);
		}
	}

	void Editable::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::MOUSE_DOWN)
		{
			MouseParams& params = getMouseParams(message.getParam());
			mouseDown(params);
		}
		else if(message.getID() == MessageID::MOUSE_MOVE)
		{
			MouseParams& params = getMouseParams(message.getParam());
			mouseMove(params);
		}
		else if (message.getID() == MessageID::MOUSE_UP)
		{
			_translation = false;
			_rotation = false;
			_scale = false;
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			const OBB& shape = *static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE)));
			_positiveXScale = OBB(shape.getCenter() + shape.getAxisX() * (shape.getRadiusX() + 5.0f), shape.getAxisX(), Vector(5.0f, shape.getRadiusY() + 10.0f));
			_negativeXScale = OBB(shape.getCenter() - shape.getAxisX() * (shape.getRadiusX() + 5.0f), shape.getAxisX(), Vector(5.0f, shape.getRadiusY() + 10.0f));
			_positiveYScale = OBB(shape.getCenter() + shape.getAxisY() * (shape.getRadiusY() + 5.0f), shape.getAxisX(), Vector(shape.getRadiusX() + 10.0f, 5.0f));
			_negativeYScale = OBB(shape.getCenter() - shape.getAxisY() * (shape.getRadiusY() + 5.0f), shape.getAxisX(), Vector(shape.getRadiusX() + 10.0f, 5.0f));
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			if(_translation || _scale || _rotation)
			{
				Graphics::get().getSpriteBatch().add(_positiveXScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
				Graphics::get().getSpriteBatch().add(_negativeXScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
				Graphics::get().getSpriteBatch().add(_positiveYScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
				Graphics::get().getSpriteBatch().add(_negativeYScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
			}
		}
	}
}