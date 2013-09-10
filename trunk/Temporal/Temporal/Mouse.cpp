#include "Mouse.h"
#include "Graphics.h"
#include <SDL.h>

namespace Temporal
{
	void updatePosition(Vector& position, SDL_Event& e)
	{
		position.setX(e.button.x);
		position.setY(Graphics::get().getLogicalView().getY() - e.button.y);
	}

	Mouse::Mouse() : _position(Vector::Zero)
	{
		for(int i = 0; i < MouseButton::SIZE; ++i)
			_buttons[i] = ButtonState::NONE;
		_buttonsMap[SDL_BUTTON_LEFT] = MouseButton::LEFT;
		_buttonsMap[SDL_BUTTON_MIDDLE] = MouseButton::MIDDLE;
		_buttonsMap[SDL_BUTTON_RIGHT] = MouseButton::RIGHT;
	}

	void Mouse::update()
	{
		for(int i = 0; i < MouseButton::SIZE; ++i)
		{
			if(_buttons[i] == ButtonState::START_PUSHING)
			{
				_buttons[i] = ButtonState::PUSHING;
			}
			else if(_buttons[i] == ButtonState::STOP_PUSHING)
			{
				_buttons[i] = ButtonState::NONE;
			}

		}
	}

	void Mouse::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			updatePosition(_position, e);
			_buttons[_buttonsMap[e.button.button]] = ButtonState::START_PUSHING;

		}
		else if(e.type == SDL_MOUSEBUTTONUP)
		{
			updatePosition(_position, e);
			_buttons[_buttonsMap[e.button.button]] = ButtonState::STOP_PUSHING;
		}
		else if(e.type == SDL_MOUSEMOTION)
		{
			updatePosition(_position, e);
		}
	}
}
