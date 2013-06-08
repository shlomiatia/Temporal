#include "Mouse.h"
#include <SDL.h>

namespace Temporal
{
	void updatePosition(Vector& position, SDL_Event& e)
	{
		position.setX(e.button.x);
		position.setY(768.0f - e.button.y);
	}

	Mouse::Mouse() : _position(Vector::Zero)
	{
		for(int i = 0; i < MouseButton::SIZE; ++i)
			_buttons[i] = false;
		_buttonsMap[SDL_BUTTON_LEFT] = MouseButton::LEFT;
		_buttonsMap[SDL_BUTTON_MIDDLE] = MouseButton::MIDDLE;
		_buttonsMap[SDL_BUTTON_RIGHT] = MouseButton::RIGHT;
	}

	void Mouse::update(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			updatePosition(_position, e);
			_buttons[_buttonsMap[e.button.button]] = true;

		}
		else if(e.type == SDL_MOUSEBUTTONUP)
		{
			updatePosition(_position, e);
			_buttons[_buttonsMap[e.button.button]] = false;
		}
		else if(e.type == SDL_MOUSEMOTION)
		{
			updatePosition(_position, e);
		}
	}
}
