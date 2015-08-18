#include "Control.h"
#include "Layer.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include "Graphics.h"
#include "Shapes.h"
#include "ShapeOperations.h"
#include "Font.h"
#include <SDL_opengl.h>

namespace Temporal
{
	const Hash Control::TYPE = Hash("control");

	void deleteEvent(IAction1<const MouseParams&>* e)
	{
		if(e)
			delete e;
	}

	void raiseEvent(IAction1<const MouseParams&>* e, const MouseParams& params)
	{
		if(e)
			(*e)(params);
	}

	void raiseMouseDown(IAction1<const MouseParams&>* mouseDownEvent, const MouseParams& params, bool& isDown, bool& isClick)
	{
		isClick = true;
		isDown = true;
		raiseEvent(mouseDownEvent, params);
	}

	void raiseMouseUp(IAction1<const MouseParams&>* mouseClickEvent, IAction1<const MouseParams&>* mouseUpEvent, MouseParams& params, bool& isDown, bool& isClick)
	{
		if(isClick) 
		{
			raiseEvent(mouseClickEvent, params);
			params.setHandled(true);
		}
		if(isDown)
		{
			raiseEvent(mouseUpEvent, params);
			params.setHandled(true);
		}
		isClick = false;
		isDown = false;
	}

	void Control::setTextChangedEvent(IAction1<const char*>* textChangedEvent)
	{
		_isTextBox = true;
		if(_textChangedEvent)
			delete _textChangedEvent;
		_textChangedEvent = textChangedEvent;
	}

	void Control::setCommandEvent(IAction* commandEvent)
	{
		if(_commandEvent)
			delete _commandEvent;
		_commandEvent = commandEvent;
	}

	void Control::setEvent(IAction1<const MouseParams&>*& prop, IAction1<const MouseParams&>* value)
	{
		deleteEvent(prop);
		prop = value;
	}

	Control::~Control()
	{
		deleteEvent(_leftMouseDownEvent);
		deleteEvent(_leftMouseClickEvent);
		deleteEvent(_leftMouseUpEvent);
		deleteEvent(_middleMouseDownEvent);
		deleteEvent(_middleMouseClickEvent);
		deleteEvent(_middleMouseUpEvent);
		deleteEvent(_rightMouseDownEvent);
		deleteEvent(_rightMouseClickEvent);
		deleteEvent(_rightMouseUpEvent);
		deleteEvent(_mouseMoveEvent);
		if(_textChangedEvent)
			delete _textChangedEvent;
		if(_commandEvent)
			delete _commandEvent;
	}

	void Control::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
			getEntity().getManager().getGameState().getLayersManager().addGUI(this);
			_font = ResourceManager::get().getFont(_fontFamily.c_str(), _fontSize);
			_box.setOBB(_obb);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			drawControl();
		}
		else if(message.getID() == MessageID::DRAW_TEXT)
		{
			drawText();
		}
		else if(message.getID() == MessageID::DRAW_BORDERS)
		{
			drawBorder();
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			const Vector& position = getPosition(*this);
			_obb.setCenter(position);
			message.setParam(&_obb);
		}
		else if(message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if(!_isTextBoxMode)
			{
				buttonKeyUp(key);
			}
			else
			{
				textBoxKeyUp(key);
			}
		}
		else if(message.getID() == MessageID::MOUSE_DOWN)
		{
			MouseParams& params = getMouseParams(message.getParam());
			mouseDown(params);
		}
		else if(message.getID() == MessageID::MOUSE_UP)
		{
			MouseParams& params = getMouseParams(message.getParam());
			mouseUp(params);
		}
		else if(message.getID() == MessageID::MOUSE_MOVE)
		{
			MouseParams& params = getMouseParams(message.getParam());
			mouseMove(params);
		}
	}

	void Control::mouseDown(MouseParams& params)
	{
		const OBBAABBWrapper shape(static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE))));
		params.setSender(this);
		if(intersects(shape.getOBB(), params.getPosition()))
		{
			params.setHandled(true);
			if(params.getButton() == MouseButton::LEFT)
				raiseMouseDown(_leftMouseDownEvent, params, _isLeftDown, _isLeftClick);
			else if(params.getButton() == MouseButton::MIDDLE)
				raiseMouseDown(_middleMouseDownEvent, params, _isMiddleDown, _isMiddleClick);
			else if(params.getButton() == MouseButton::RIGHT)
				raiseMouseDown(_rightMouseDownEvent, params, _isRightDown, _isRightClick);
			else
				params.setHandled(false);
		}
	}

	void Control::mouseUp(MouseParams& params)
	{
		params.setSender(this);
		if(params.getButton() == MouseButton::RIGHT)
		{
			raiseMouseUp(_rightMouseClickEvent, _rightMouseUpEvent, params, _isRightDown, _isRightClick);
		}
		else if(params.getButton() == MouseButton::MIDDLE)
		{
			raiseMouseUp(_middleMouseClickEvent, _middleMouseUpEvent, params, _isMiddleDown, _isMiddleClick);
		}
		else
		{
			if(_isLeftClick)
			{
				if(_isTextBox)
				{
					getEntity().getManager().setFocusInputComponent(this);
					_isTextBoxMode = true;
				}
				if(_commandEvent)
					(*_commandEvent)();

			}
			raiseMouseUp(_leftMouseClickEvent, _leftMouseUpEvent, params, _isLeftDown, _isLeftClick);
		}
	}

	void Control::mouseMove(MouseParams& params)
	{
		_isLeftClick = false;
		_isMiddleClick = false;
		_isRightClick = false;
		params.setSender(this);
		if(_isLeftDown || _isMiddleDown || _isRightDown)
		{
			params.setHandled(true);
			raiseEvent(_mouseMoveEvent, params);
		}
		if(_leftMouseClickEvent || _commandEvent || _textChangedEvent)
		{
			const OBBAABBWrapper shape(static_cast<OBB*>(raiseMessage(Message(MessageID::GET_SHAPE))));
			_isHover = intersects(shape.getOBB(), params.getPosition());
		}
	}

	void Control::buttonKeyUp(Key::Enum key)
	{
		if(key != Key::NONE && key == _shortcutKey)
		{
			if(_commandEvent)
				(*_commandEvent)();
		}
	}

	void Control::textBoxKeyUp(Key::Enum key)
	{
		if(key == Key::ENTER)
		{
			_isTextBoxMode = false;
			getEntity().getManager().clearFocusInputComponent();
			if(_textChangedEvent)
				(*_textChangedEvent)(_textbox.c_str());
		}
		else if(key == Key::BACKSPACE)
		{
			if (_textbox.size() > 0)
				_textbox.resize(_textbox.size () - 1);
		}
		else if(key != Key::NONE)
		{
			_textbox += static_cast<char>(key);
		}
	}
	
	void Control::drawControl()
	{
		const Vector& position = getPosition(*this);
		_obb.setCenter(position);
		const Color& color = _isHover ? _hoverColor : _backgroundColor;
		Graphics::get().getSpriteBatch().add(_box.getOBB(), color);

	}

	void Control::drawText()
	{
		const Vector& position = getPosition(*this);
		_font->draw(getString().c_str(), position, _foregroundColor);
	}

	void Control::drawBorder()
	{
		Graphics::get().getLinesSpriteBatch().add(_box.getOBB(), _borderColor);
	}
}