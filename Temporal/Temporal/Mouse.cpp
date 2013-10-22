#include "Mouse.h"
#include "Graphics.h"
#include "GameState.h"
#include "EntitySystem.h"
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
			GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MOUSE_DOWN, &params));
		}
		else if(e.type == SDL_MOUSEMOTION)
		{
			Vector position = getPosition(e);
			MouseParams params(MouseButton::NONE, position);
			GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MOUSE_MOVE, &params));
		}
	}

	void MouseListener::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::MOUSE_DOWN)
		{
			const YABP& shape = *static_cast<YABP*>(_owner.raiseMessage(Message(MessageID::GET_SHAPE)));
			const MouseParams& params = getMouseParams(message.getParam());
			if(intersects(shape, params.getPosition()))
			{
				_isClick = true;
				_isDown= true;
				mouseDown(params.getButton());
			}
		}
		else if(message.getID() == MessageID::MOUSE_UP)
		{
			const MouseParams& params = getMouseParams(message.getParam());
			if(_isClick)
				mouseClick(params.getButton());
			if(_isDown)
				mouseUp(params.getButton());
			_isClick = false;
			_isDown = false;
		}
		else if(message.getID() == MessageID::MOUSE_MOVE)
		{
			_isClick= false;
		}
	}
}
