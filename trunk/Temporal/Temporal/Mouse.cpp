#include "Mouse.h"
#include "Graphics.h"
#include "GameState.h"
#include "MessageUtils.h"
#include "ShapeOperations.h"

#include <SDL.h>

namespace Temporal
{
	Vector getPosition(SDL_Event& e)
	{
		float x = e.button.x * Graphics::get().getLogicalView().getX() / Graphics::get().getResolution().getX();
		float y = Graphics::get().getLogicalView().getY() - e.button.y * Graphics::get().getLogicalView().getY() / Graphics::get().getResolution().getY();
		Vector position(x, y);
		return position;
	}

	Mouse::Mouse()
	{
		_buttonsMap[SDL_BUTTON_LEFT] = MouseButton::LEFT;
		_buttonsMap[SDL_BUTTON_MIDDLE] = MouseButton::MIDDLE;
		_buttonsMap[SDL_BUTTON_RIGHT] = MouseButton::RIGHT;
		_buttonsMap[SDL_BUTTON_WHEELDOWN] = MouseButton::WHEEL_DOWN;
		_buttonsMap[SDL_BUTTON_WHEELUP] = MouseButton::WHEEL_UP;
	}

	void Mouse::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			MouseButton::Enum button = _buttonsMap[e.button.button];
			Vector position = getPosition(e);
			MouseParams params(button, position);
			GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MOUSE_DOWN, &params));
		}
		else if(e.type == SDL_MOUSEBUTTONUP)
		{
			MouseButton::Enum button = _buttonsMap[e.button.button];
			Vector position = getPosition(e);
			MouseParams params(button, position);
			GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MOUSE_UP, &params));
		}
		else if(e.type == SDL_MOUSEMOTION)
		{
			Vector position = getPosition(e);
			MouseParams params(MouseButton::NONE, position);
			GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MOUSE_MOVE, &params));
		}
	}

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

	void MouseListener::setEvent(IAction1<const MouseParams&>*& prop, IAction1<const MouseParams&>* value)
	{
		deleteEvent(prop);
		prop = value;
	}

	MouseListener::~MouseListener()
	{
		deleteEvent(_leftMouseDownEvent);
		deleteEvent(_leftMouseClickEvent);
		deleteEvent(_leftMouseUpEvent);
		deleteEvent(_rightMouseDownEvent);
		deleteEvent(_rightMouseClickEvent);
		deleteEvent(_rightMouseUpEvent);
		deleteEvent(_mouseMoveEvent);
	}

	void MouseListener::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::MOUSE_DOWN)
		{
			const YABP& shape = *static_cast<YABP*>(raiseMessage(Message(MessageID::GET_SHAPE)));
			const MouseParams& params = getMouseParams(message.getParam());
			if(intersects(shape, params.getPosition()))
			{
				if(params.getButton() == MouseButton::LEFT)
					mouseDown(_leftMouseDownEvent, params, _isLeftDown, _isLeftClick);
				if(params.getButton() == MouseButton::RIGHT)
					mouseDown(_rightMouseDownEvent, params, _isRightDown, _isRightClick);
			}
		}
		else if(message.getID() == MessageID::MOUSE_UP)
		{
			const MouseParams& params = getMouseParams(message.getParam());
			if(params.getButton() == MouseButton::LEFT)
				mouseUp(_leftMouseClickEvent, _leftMouseUpEvent, params, _isLeftDown, _isLeftClick);
			else
				mouseUp(_rightMouseClickEvent, _rightMouseUpEvent, params, _isRightDown, _isRightClick);
			
		}
		else if(message.getID() == MessageID::MOUSE_MOVE)
		{
			_isLeftClick = false;
			_isRightClick = false;
			if(_isLeftDown || _isRightDown)
				raiseEvent(_mouseMoveEvent, getMouseParams(message.getParam()));
		}
	}
}
