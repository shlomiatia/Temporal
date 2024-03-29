#include "Editable.h"
#include "MessageUtils.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "Layer.h"
#include "StaticBody.h"
#include "Fixture.h"
#include "Grid.h"
#include "ToolsUtils.h"
#include "Renderer.h"
#include "SpriteSheet.h"
#include "SceneNode.h"
#include "Texture.h"
#include "TemporalPeriod.h"
#include "Math.h"
#include "GameStateEditor.h"

namespace Temporal
{
	static const float MARGIN = 5.0f;
	static const Hash STATIC_BODY_TYPE("static-body");
	
	void fixFlatOBB(OBB& obb)
	{
		if (obb.getHeight() == 0.0f)
			obb.setRadiusY(MARGIN);
	}

	void Editable::reset()
	{
		_translation = false;
		_rotation = false;
		_scale = false;
		_editor.addUndo();
	}

	OBB Editable::getShape() const
	{
		return getShape(getEntity());
	}

	OBB Editable::getShape(const Entity& entity)
	{
		OBB* shape = static_cast<OBB*>(entity.handleMessage(Message(MessageID::GET_SHAPE)));
		if (shape)
		{
			return *shape;
		}
		if (const Renderer* renderer = static_cast<const Renderer*>(entity.get(ComponentsIds::RENDERER)))
		{
			const SceneNode& root = renderer->getRootSceneNode();
			const SpriteSheet& spriteSheet = renderer->getSpriteSheet();
			const Vector& position = getPosition(entity);
			float rotation = AngleUtils::degreesToRadians(root.getRotation());
			const Vector& size = spriteSheet.getTexture().getSize();
			Vector radius = Vector(size.getX() / 2.0f * root.getScale().getX(), size.getY() / 2.0f * root.getScale().getY());

			OBB shape(position, rotation, radius);
			return shape;
		}
		else
		{
			const Vector& position = getPosition(entity);
			return OBBAABB(position, Vector(MARGIN, MARGIN));
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
		Renderer* renderer = static_cast<Renderer*>(getEntity().get(ComponentsIds::RENDERER));
		if (renderer)
		{
			SceneNode& root = renderer->getRootSceneNode();
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
		Renderer* renderer = static_cast<Renderer*>(getEntity().get(ComponentsIds::RENDERER));
		if (renderer)
		{
			SceneNode& root = renderer->getRootSceneNode();
			const SpriteSheet& spriteSheet = renderer->getSpriteSheet();
			const Vector& size = spriteSheet.getTexture().getSize();
			Vector scale((radius.getX() == 0.0f ? 1.0f : radius.getX()) / (size.getX() / 2.0f), (radius.getY() == 0.0f ? 1.0f : radius.getY()) / (size.getY() / 2.0f));
			root.setScale(scale);
		}
		
	}

	bool isCloserThenSelected(Editable& editable, GameStateEditor& editor)
	{
		const Renderer* selectedRenderer = static_cast<const Renderer*>(editor.getSelected()->getEntity().get(ComponentsIds::RENDERER));
		const Renderer* editableRenderer = static_cast<const Renderer*>(editable.getEntity().get(ComponentsIds::RENDERER));
		return (selectedRenderer && editableRenderer && selectedRenderer->getLayer() >= editableRenderer->getLayer());
	}

	void Editable::leftMouseDown(MouseParams& params)
	{
		if (_editor.getSelected() && _editor.getSelected() != this && isCloserThenSelected(*this, _editor))
		{
			OBB selectedShape = _editor.getSelected()->getShape();
			fixFlatOBB(selectedShape);
			selectedShape.setRadiusX(selectedShape.getRadiusX() + MARGIN * 2.0f);
			selectedShape.setRadiusY(selectedShape.getRadiusY() + MARGIN * 2.0f);
			if (intersects(selectedShape, params.getPosition()))
				return;
		}

		OBB shape = getShape();
		fixFlatOBB(shape);
		if (intersects(shape, params.getPosition()))
		{
			_editor.setSelected(this);
			reset();
			_translation = true;
			_translationOffset = params.getPosition() - getPosition(*this);
			params.setHandled(true);
		}
		else if (!_translationOnly)
		{
			if (intersects(_positiveXScale, params.getPosition()))
			{
				_editor.setSelected(this);
				reset();
				_scale = true;
				_isPositiveScale = true;
				_scaleAxis = Axis::X;
				params.setHandled(true);
			}
			else if (intersects(_negativeXScale, params.getPosition()))
			{
				_editor.setSelected(this);
				reset();
				_scale = true;
				_isPositiveScale = false;
				_scaleAxis = Axis::X;
				params.setHandled(true);
			}
			else if (intersects(_positiveYScale, params.getPosition()))
			{
				_editor.setSelected(this);
				reset();
				_scale = true;
				_isPositiveScale = true;
				_scaleAxis = Axis::Y;
				params.setHandled(true);
			}
			else if (intersects(_negativeYScale, params.getPosition()))
			{
				_editor.setSelected(this);
				reset();
				_scale = true;
				_isPositiveScale = false;
				_scaleAxis = Axis::Y;
				params.setHandled(true);
			}
			else if (_editor.getSelected() == this)
			{
				_editor.setSelected(0);
			}
		}
	}

	void Editable::rightMouseDown(MouseParams& params)
	{
		if (!_translationOnly && _editor.getSelected() == this)
		{
			reset();
			_rotation = true;
			params.setHandled(true);
		}
	}

	void Editable::middleMouseDown(MouseParams& params)
	{
		if (_editor.getSelected() == this)
		{
			_editor.addUndo();
			raiseMessage(Message(MessageID::FLIP_ORIENTATION));
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
		else if (params.getButton() == MouseButton::MIDDLE)
		{
			middleMouseDown(params);
		}
	}

	void Editable::mouseMove(MouseParams& params)
	{
		if(_translation)
		{
			OBB shape = getShape();
			float tileSize = getEntity().getManager().getGameState().getGrid().getTileSize();
			Vector newPosition = params.getPosition() - _translationOffset;
			newPosition.setX(ToolsUtils::snap(newPosition.getX(), tileSize / 4.0f, 8.0f, shape.getRadiusX()));
			newPosition.setY(ToolsUtils::snap(newPosition.getY(), tileSize / 4.0f, 8.0f, shape.getRadiusY()));
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			params.setHandled(true);
		}
		else if(_rotation)
		{
			Vector vector = (params.getPosition() - getPosition(*this)).normalize();
			float angle = ToolsUtils::snap(vector.getAngle(), AngleUtils::radian().ANGLE_15_IN_RADIANS, AngleUtils::degreesToRadians(5.0f));
			if (fabsf(angle) < EPSILON)
			{
				angle = 0.0f;
			}
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
			radius.setAxis(_scaleAxis, ToolsUtils::snap(axisRadius + delta, getEntity().getManager().getGameState().getGrid().getTileSize() / 8.0f, 4.0f));
			delta = radius.getAxis(_scaleAxis) - axisRadius;
			if (radius.getX() < 0.0f)
				radius.setX(0.0f);
			if (radius.getY() < 0.0f)
				radius.setY(0.0f);

			setRadius(radius);
			
			Vector newPosition = position + axis * delta;
			raiseMessage(Message(MessageID::SET_POSITION, &newPosition));
			params.setHandled(true);
		}
	}

	void Editable::handleArrows(const Vector& params)
	{
		Vector position = getPosition(*this);
		position += params;
		raiseMessage(Message(MessageID::SET_POSITION, &position));
	}

	void Editable::removePeriod()
	{
		_editor.addUndo();
		getEntity().remove(ComponentsIds::TEMPORAL_PERIOD);
	}

	void Editable::setPeriod(int period)
	{
		_editor.addUndo();
		Component* component = getEntity().get(ComponentsIds::TEMPORAL_PERIOD);
		if (!component)
		{
			component = new TemporalPeriod();
			getEntity().add(component);
		}
		TemporalPeriod* temporalPeriod = static_cast<TemporalPeriod*>(component);
		temporalPeriod->setPeriod(static_cast<Period::Enum>(period));
	}

	void Editable::keyUp(Key::Enum key)
	{
		if (key == Key::UP)
		{
			handleArrows(Vector(0.0f, 1.0f));
		}
		else if (key == Key::DOWN)
		{
			handleArrows(Vector(0.0f, -1.0f));
		}
		else if (key == Key::LEFT)
		{
			handleArrows(Vector(-1.0f, 0.0f));
		}
		else if (key == Key::RIGHT)
		{
			handleArrows(Vector(1.0f, 0.0f));
		}
		else if (key == Key::BACKSPACE)
		{
			removePeriod();
		}
		else if (key == Key::OPEN_ANGLE_BRACKET)
		{
			setPeriod(Period::PAST);
		}
		else if (key == Key::CLOSE_ANGLE_BRACKET)
		{
			setPeriod(Period::PRESENT);
		}
	}

	void Editable::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			if (!_translationOnly)
			{
				OBB shape = getShape();
				setRotation(shape.getAngle());
				setRadius(shape.getRadius());
			}
		}
		else if(message.getID() == MessageID::MOUSE_DOWN)
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
			float radius = _translationOnly ? 1.0f : MARGIN;
			OBB shape = getShape();
			fixFlatOBB(shape);
			_positiveXScale = OBB(shape.getCenter() + shape.getAxisX() * (shape.getRadiusX() + radius), shape.getAxisX(), Vector(radius, shape.getRadiusY() + radius * 2.0f));
			_negativeXScale = OBB(shape.getCenter() - shape.getAxisX() * (shape.getRadiusX() + radius), shape.getAxisX(), Vector(radius, shape.getRadiusY() + radius * 2.0f));
			_positiveYScale = OBB(shape.getCenter() + shape.getAxisY() * (shape.getRadiusY() + radius), shape.getAxisX(), Vector(shape.getRadiusX() + radius * 2.0f, radius));
			_negativeYScale = OBB(shape.getCenter() - shape.getAxisY() * (shape.getRadiusY() + radius), shape.getAxisX(), Vector(shape.getRadiusX() + radius * 2.0f, radius));
			
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			if (_editor.getSelected() == this)
			{
				Color color = _translationOnly ? Color(1.0f, 1.0f, 1.0f, 0.75f) : Color(1.0f, 1.0f, 1.0f, 0.5f);
				Graphics::get().getLinesSpriteBatch().add(_positiveXScale, color);
				Graphics::get().getLinesSpriteBatch().add(_negativeXScale, color);
				Graphics::get().getLinesSpriteBatch().add(_positiveYScale, color);
				Graphics::get().getLinesSpriteBatch().add(_negativeYScale, color);
			}
		}
		else if (message.getID() == MessageID::KEY_UP)
		{
			if (this != _editor.getSelected())
				return;
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			keyUp(key);
		}
	}
}