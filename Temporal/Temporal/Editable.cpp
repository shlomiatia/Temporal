#include "Editable.h"
#include "MessageUtils.h"
#include "Mouse.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "Layer.h"
#include "StaticBody.h"
#include "Fixture.h"
#include "Grid.h"
#include "Math.h"
#include "Renderer.h"
#include "SpriteSheet.h"
#include "SceneNode.h"
#include "Texture.h"

namespace Temporal
{
	static Hash STATIC_BODY_TYPE("static-body");
	const Hash Editable::TYPE("editable");
	Editable* Editable::_selected = 0;

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

	OBB Editable::getShape() const
	{
		OBB* shape = static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE)));
		if (shape)
		{
			return *shape;
		}
		else
		{
			const Renderer& renderer = *static_cast<const Renderer*>(getEntity().get(Renderer::TYPE));
			const SceneNode& root = renderer.getRootSceneNode();
			const SpriteSheet& spriteSheet = renderer.getSpriteSheet();
			const Vector& position = getPosition(*this);
			float rotation = AngleUtils::degreesToRadians(root.getRotation());
			const Vector& size = spriteSheet.getTexture().getSize();
			Vector radius = Vector(size.getX() / 2.0f * root.getScale().getX(), size.getY() / 2.0f * root.getScale().getY());

			OBB shape(position, rotation, radius);
			return shape;

		}	
	}

	void Editable::setRotation(float rotation)
	{
		StaticBody* staticBody = static_cast<StaticBody*>(getEntity().get(STATIC_BODY_TYPE));
		if (staticBody)
		{

			staticBody->getFixture().getLocalShape().setAxis0(Vector(rotation));
			getEntity().getManager().getGameState().getGrid().update(&staticBody->getFixture());
		}
		else
		{
			Renderer& renderer = *static_cast<Renderer*>(getEntity().get(Renderer::TYPE));
			SceneNode& root = renderer.getRootSceneNode();
			root.setRotation(AngleUtils::radiansToDegrees(rotation));
		}
		
	}

	void Editable::setRadius(const Vector& radius)
	{
		StaticBody* staticBody = static_cast<StaticBody*>(getEntity().get(STATIC_BODY_TYPE));
		if (staticBody)
		{
			staticBody->getFixture().getLocalShape().setRadius(radius);
			getEntity().getManager().getGameState().getGrid().update(&staticBody->getFixture());
		}
		else
		{
			Renderer& renderer = *static_cast<Renderer*>(getEntity().get(Renderer::TYPE));
			SceneNode& root = renderer.getRootSceneNode();
			const SpriteSheet& spriteSheet = renderer.getSpriteSheet();
			const Vector& size = spriteSheet.getTexture().getSize();
			Vector scale(radius.getX() / (size.getX() / 2.0f), radius.getY() / (size.getY() / 2.0f));
			root.setScale(scale);
		}
		
	}

	void Editable::leftMouseDown(MouseParams& params)
	{
		OBB shape = getShape();
		if (shape.getHeight() == 0.0f)
			shape.setRadiusY(5.0f);
		if (intersects(shape, params.getPosition()))
		{
			_selected = this;
			reset();
			_translation = true;
			_translationOffset = params.getPosition() - getPosition(*this);
			params.setHandled(true);
		}
		else if (!_translationOnly)
		{
			if (intersects(_positiveXScale, params.getPosition()))
			{
				reset();
				_scale = true;
				_isPositiveScale = true;
				_scaleAxis = Axis::X;
				params.setHandled(true);
			}
			else if (intersects(_negativeXScale, params.getPosition()))
			{
				reset();
				_scale = true;
				_isPositiveScale = false;
				_scaleAxis = Axis::X;
				params.setHandled(true);
			}
			else if (intersects(_positiveYScale, params.getPosition()))
			{
				reset();
				_scale = true;
				_isPositiveScale = true;
				_scaleAxis = Axis::Y;
				params.setHandled(true);
			}
			else if (intersects(_negativeYScale, params.getPosition()))
			{
				reset();
				_scale = true;
				_isPositiveScale = false;
				_scaleAxis = Axis::Y;
				params.setHandled(true);
			}
			else if (_selected == this)
			{
				_selected = 0;
			}
		}
	}

	void Editable::rightMouseDown(MouseParams& params)
	{
		if (!_translationOnly && _selected == this)
		{
			reset();
			_rotation = true;
			params.setHandled(true);
		}
	}

	void Editable::mouseDown(MouseParams& params)
	{
		if (params.getButton() == MouseButton::LEFT)
		{
			leftMouseDown(params);
		}
		else if (params.getButton() == MouseButton::RIGHT)
		{
			rightMouseDown(params);
		}
	}

	float snap(float val, float target, float maxSnap)
	{
		float valModTarget = fmodf(val, target);
		float targetMinusValModTarget = target - valModTarget;
		if (valModTarget <= maxSnap)
			return val - valModTarget;
		else if (targetMinusValModTarget <= maxSnap)
			return val + targetMinusValModTarget;
		else
			return val;
	}

	void Editable::mouseMove(MouseParams& params)
	{
		if(_translation)
		{
			Vector newPosition = params.getPosition() - _translationOffset;
			newPosition.setX(snap(newPosition.getX(), getEntity().getManager().getGameState().getGrid().getTileSize() / 4.0f, 10.0f));
			newPosition.setY(snap(newPosition.getY(), getEntity().getManager().getGameState().getGrid().getTileSize() / 4.0f, 10.0f));
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			params.setHandled(true);
		}
		else if(_rotation)
		{
			Vector vector = (params.getPosition() - getPosition(*this)).normalize();
			float angle = snap(vector.getAngle(), AngleUtils::radian().ANGLE_15_IN_RADIANS, AngleUtils::degreesToRadians(5.0f));
			setRotation(angle);
			params.setHandled(true);
		}
		else if(_scale)
		{
			Vector position = getPosition(*this);
			Vector vector = params.getPosition() - position;
			OBB shape = getShape();
			Vector axis = shape.getAxis(_scaleAxis);
			if(!_isPositiveScale)
				axis = -axis;
			Vector radius = shape.getRadius();
			float axisRadius = radius.getAxis(_scaleAxis);
			float delta = (vector * axis - axisRadius) / 2.0f;
			radius.setAxis(_scaleAxis, snap(axisRadius + delta, getEntity().getManager().getGameState().getGrid().getTileSize() / 8.0f, 10.0f));
			delta = radius.getAxis(_scaleAxis) - axisRadius;

			setRadius(radius);
			
			Vector newPosition = position + axis * delta;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			params.setHandled(true);
		}
	}

	void Editable::onInit() 
	{
		if (getEntity().get(Hash("camera-control")))
		{
			Renderer* renderer = new Renderer("resources/textures/camera-logo-hi.png", "", 0, LayerType::PARTICLES, Color(1.0f, 1.0f, 1.0f, 0.1f));
			renderer->setBypassSave(true);
			getEntity().add(renderer);
			renderer->handleMessage(Message(MessageID::ENTITY_INIT));
		}
	};


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
			if (!_translationOnly)
			{
				OBB shape = getShape();
				_positiveXScale = OBB(shape.getCenter() + shape.getAxisX() * (shape.getRadiusX() + 5.0f), shape.getAxisX(), Vector(5.0f, shape.getRadiusY() + 10.0f));
				_negativeXScale = OBB(shape.getCenter() - shape.getAxisX() * (shape.getRadiusX() + 5.0f), shape.getAxisX(), Vector(5.0f, shape.getRadiusY() + 10.0f));
				_positiveYScale = OBB(shape.getCenter() + shape.getAxisY() * (shape.getRadiusY() + 5.0f), shape.getAxisX(), Vector(shape.getRadiusX() + 10.0f, 5.0f));
				_negativeYScale = OBB(shape.getCenter() - shape.getAxisY() * (shape.getRadiusY() + 5.0f), shape.getAxisX(), Vector(shape.getRadiusX() + 10.0f, 5.0f));
			}
			
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			if (!_translationOnly && _selected == this)
			{
				Graphics::get().getSpriteBatch().add(_positiveXScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
				Graphics::get().getSpriteBatch().add(_negativeXScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
				Graphics::get().getSpriteBatch().add(_positiveYScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
				Graphics::get().getSpriteBatch().add(_negativeYScale, Color(1.0f, 1.0f, 1.0f, 0.5f));
			}
		}
	}
}