#include "Control.h"
#include "Layer.h"
#include "Vector.h"
#include "MessageUtils.h"
#include "ResourceManager.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Mouse.h"
#include "ShapeOperations.h"
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

	void mouseDown(IAction1<const MouseParams&>* mouseDownEvent, const MouseParams& params, bool& isDown, bool& isClick)
	{
		isClick = true;
		isDown = true;
		raiseEvent(mouseDownEvent, params);
	}

	void mouseUp(IAction1<const MouseParams&>* mouseClickEvent, IAction1<const MouseParams&>* mouseUpEvent, const MouseParams& params, bool& isDown, bool& isClick)
	{
		if(isClick)
			raiseEvent(mouseClickEvent, params);
		if(isDown)
			raiseEvent(mouseUpEvent, params);
		isClick = false;
		isDown = false;
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
		deleteEvent(_rightMouseDownEvent);
		deleteEvent(_rightMouseClickEvent);
		deleteEvent(_rightMouseUpEvent);
		deleteEvent(_mouseMoveEvent);
	}

	void Control::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			
			getEntity().getManager().getGameState().getLayersManager().addGUI(this);
			_font = ResourceManager::get().getFont(_fontFamily.c_str(), _fontSize);
			_layout.SetFont(_font.get());
			_layout.SetAlignment(FTGL::TextAlignment::ALIGN_CENTER);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			draw();
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			const Vector& position = getPosition(*this);
			_box.setCenter(position);
			message.setParam(&_box);
		}
		else if(message.getID() == MessageID::SET_TEXT)
		{
			const char* text = static_cast<const char*>(message.getParam());
			setText(text);
		}
		else if(message.getID() == MessageID::MOUSE_DOWN)
		{
			const YABP& shape = *static_cast<YABP*>(raiseMessage(Message(MessageID::GET_SHAPE)));
			MouseParams& params = getMouseParams(message.getParam());
			params.setSender(this);
			if(intersects(shape, params.getPosition()))
			{
				//params.setHandled(true);
				if(params.getButton() == MouseButton::LEFT)
					mouseDown(_leftMouseDownEvent, params, _isLeftDown, _isLeftClick);
				else if(params.getButton() == MouseButton::RIGHT)
					mouseDown(_rightMouseDownEvent, params, _isRightDown, _isRightClick);
				/*else
					params.setHandled(false);*/
			}
		}
		else if(message.getID() == MessageID::MOUSE_UP)
		{
			MouseParams& params = getMouseParams(message.getParam());
			params.setSender(this);
			//params.setHandled(true);
			if(params.getButton() == MouseButton::LEFT)
				mouseUp(_leftMouseClickEvent, _leftMouseUpEvent, params, _isLeftDown, _isLeftClick);
			else
				mouseUp(_rightMouseClickEvent, _rightMouseUpEvent, params, _isRightDown, _isRightClick);
		}
		else if(message.getID() == MessageID::MOUSE_MOVE)
		{
			_isLeftClick = false;
			_isRightClick = false;
			MouseParams& params = getMouseParams(message.getParam());
			params.setSender(this);
			if(_isLeftDown || _isRightDown)
			{
//				params.setHandled(true);
				raiseEvent(_mouseMoveEvent, params);
			}
		}
	}
	
	void Control::draw()
	{
		const Vector& position = getPosition(*this);
		_box.setCenter(position);
		Graphics::get().draw(_box, _backgroundColor, true);
		Graphics::get().draw(_box, _borderColor, false);
		if(getString() != "")
		{
			glColor4f(_foregroundColor.getR(), _foregroundColor.getG(), _foregroundColor.getB(), _foregroundColor.getA());
			glPushMatrix();
			{
				glTranslatef(position.getX() - _box.getWidth() / 2.0f, position.getY() - _box.getHeight() / 4.0f , 0.0f);
				_layout.Render(getText());
				Graphics::get().bindTexture(0); // Because we cache textures...
			}
			glPopMatrix();
		}
	}

	void Control::setWidth(float width)
	{
		_layout.SetLineLength(width);
		_box.setWidth(width);
	}
}