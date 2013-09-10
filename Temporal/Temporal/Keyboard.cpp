#include "Keyboard.h"
#include <SDL.h>

namespace Temporal
{
	Keyboard::Keyboard()
	{
		for(int i = 0; i < Key::SIZE; ++i)
			_keys[i] = ButtonState::NONE;
		_keysMap[SDLK_ESCAPE] = Key::ESC;
		_keysMap[SDLK_F1] = Key::F1;
		_keysMap[SDLK_F2] = Key::F2;
		_keysMap[SDLK_F3] = Key::F3;
		_keysMap[SDLK_F4] = Key::F4;
		_keysMap[SDLK_F5] = Key::F5;
		_keysMap[SDLK_F6] = Key::F6;
		_keysMap[SDLK_F7] = Key::F7;
		_keysMap[SDLK_F8] = Key::F8;
		_keysMap[SDLK_F9] = Key::F9;
		_keysMap[SDLK_F10] = Key::F10;
		_keysMap[SDLK_F11] = Key::F11;
		_keysMap[SDLK_F12] = Key::F2;

		_keysMap[SDLK_GREATER] = Key::TILDE;
		_keysMap[SDLK_1] = Key::D1;
		_keysMap[SDLK_2] = Key::D2;
		_keysMap[SDLK_3] = Key::D3;
		_keysMap[SDLK_4] = Key::D4;
		_keysMap[SDLK_5] = Key::D5;
		_keysMap[SDLK_6] = Key::D6;
		_keysMap[SDLK_7] = Key::D7;
		_keysMap[SDLK_8] = Key::D8;
		_keysMap[SDLK_9] = Key::D9;
		_keysMap[SDLK_0] = Key::D0;
		_keysMap[SDLK_MINUS] = Key::MINUS;
		_keysMap[SDLK_KP_MINUS] = Key::MINUS;
		_keysMap[SDLK_PLUS] = Key::PLUS;
		_keysMap[SDLK_EQUALS] = Key::PLUS;
		_keysMap[SDLK_KP_PLUS] = Key::PLUS;
		_keysMap[SDLK_BACKSPACE] = Key::BACKSPACE;

		_keysMap[SDLK_TAB] = Key::TAB;
		_keysMap[SDLK_q] = Key::Q;
		_keysMap[SDLK_w] = Key::W;
		_keysMap[SDLK_e] = Key::E;
		_keysMap[SDLK_r] = Key::R;
		_keysMap[SDLK_t] = Key::T;
		_keysMap[SDLK_y] = Key::Y;
		_keysMap[SDLK_u] = Key::U;
		_keysMap[SDLK_i] = Key::I;
		_keysMap[SDLK_o] = Key::O;
		_keysMap[SDLK_p] = Key::P;
		_keysMap[SDLK_LEFTBRACKET] = Key::OPEN_BRACKET;
		_keysMap[SDLK_RIGHTBRACKET] = Key::CLOSE_BRACKET;
		_keysMap[SDLK_RETURN] = Key::ENTER;

		_keysMap[SDLK_CAPSLOCK] = Key::CAPS_LOCK;
		_keysMap[SDLK_a] = Key::A;
		_keysMap[SDLK_s] = Key::S;
		_keysMap[SDLK_d] = Key::D;
		_keysMap[SDLK_f] = Key::F;
		_keysMap[SDLK_g] = Key::G;
		_keysMap[SDLK_h] = Key::H;
		_keysMap[SDLK_j] = Key::J;
		_keysMap[SDLK_k] = Key::K;
		_keysMap[SDLK_l] = Key::L;
		_keysMap[SDLK_SEMICOLON] = Key::SEMICOLON;
		_keysMap[SDLK_BACKSLASH] = Key::BACKSLASH;

		_keysMap[SDLK_LSHIFT] = Key::LEFT_SHIFT;
		_keysMap[SDLK_z] = Key::Z;
		_keysMap[SDLK_x] = Key::X;
		_keysMap[SDLK_c] = Key::C;
		_keysMap[SDLK_v] = Key::V;
		_keysMap[SDLK_b] = Key::B;
		_keysMap[SDLK_n] = Key::N;
		_keysMap[SDLK_m] = Key::M;
		_keysMap[SDLK_COMMA] = Key::COMMA;
		_keysMap[SDLK_PERIOD] = Key::PERIOD;
		_keysMap[SDLK_SLASH] = Key::SLASH;

		_keysMap[SDLK_LCTRL] = Key::LEFT_CTRL;
		_keysMap[SDLK_LSUPER] = Key::WINDOWS;
		_keysMap[SDLK_LALT] = Key::LEFT_ALT;
		_keysMap[SDLK_SPACE] = Key::SPACE;
		_keysMap[SDLK_RALT] = Key::RIGHT_ALT;
		_keysMap[SDLK_RCTRL] = Key::RIGHT_CTRL;

		_keysMap[SDLK_PRINT] = Key::PRINT_SCREEN;
		_keysMap[SDLK_SCROLLOCK] = Key::SCROLL_LOCK;
		_keysMap[SDLK_PAUSE] = Key::PAUSE;
		_keysMap[SDLK_INSERT] = Key::INSERT;
		_keysMap[SDLK_HOME] = Key::HOME;
		_keysMap[SDLK_PAGEUP] = Key::PAGE_UP;
		_keysMap[SDLK_DELETE] = Key::DELETE;
		_keysMap[SDLK_END] = Key::END;
		_keysMap[SDLK_PAGEDOWN] = Key::PAGE_DOWN;

		_keysMap[SDLK_LEFT] = Key::LEFT;
		_keysMap[SDLK_UP] = Key::UP;
		_keysMap[SDLK_DOWN] = Key::DOWN;
		_keysMap[SDLK_RIGHT] = Key::RIGHT;

		_keysMap[SDLK_NUMLOCK] = Key::NUM_LOCK;
		_keysMap[SDLK_ASTERISK] = Key::ASTERISK;
	}

	void Keyboard::update()
	{
		for(int i = 0; i < Key::SIZE; ++i)
		{
			if(_keys[i] == ButtonState::START_PUSHING)
			{
				_keys[i] = ButtonState::PUSHING;
			}
			else if(_keys[i] == ButtonState::STOP_PUSHING)
			{
				_keys[i] = ButtonState::NONE;
			}

		}
	}

	void Keyboard::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_QUIT)
		{
			_keys[Key::ESC] = ButtonState::START_PUSHING;
		}
		else if (e.type == SDL_KEYDOWN)
		{
			int key = _keysMap[e.key.keysym.sym];
			_keys[key] = ButtonState::START_PUSHING;
		}
		else if (e.type == SDL_KEYUP)
		{
			int key = _keysMap[e.key.keysym.sym];
			_keys[key] = ButtonState::STOP_PUSHING;
		}
	}
}
