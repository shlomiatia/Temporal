#include "Input.h"
#include <SDL.h>

namespace Temporal
{
	void Input::update(void)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				_quit = true;
			}
			else if ((event.type == SDL_KEYDOWN) || (event.type == SDL_KEYUP))
			{
				switch (event.key.keysym.sym)
				{
					case (SDLK_UP):
					{
						_up = event.type == SDL_KEYDOWN;
						break;
					}
					case (SDLK_DOWN):
					{
						_down = event.type == SDL_KEYDOWN;
						break;
					}
					case (SDLK_LEFT):
					{
						_left = event.type == SDL_KEYDOWN;
						break;
					}
					case (SDLK_RIGHT):
					{
						_right = event.type == SDL_KEYDOWN;
						break;
					}
					case (SDLK_ESCAPE):
					{
						_quit = event.type == SDL_KEYDOWN;
						break;
					}
				}
			}
		}
	}
}
