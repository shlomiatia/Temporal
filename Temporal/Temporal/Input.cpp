#include "Input.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <SDL.h>

namespace Temporal
{
	void Input::init()
	{
		_gamepad.init();
	}

	void Input::dispose()
	{
		_gamepad.dispose();
	}

	void Input::update()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			Keyboard::get().update(&e);
			Mouse::get().update(&e);
			_gamepad.update(&e);
		}
	}
}
