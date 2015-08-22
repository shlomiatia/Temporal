#include "Keyboard.h"
#include "GameState.h"
#include "EntitySystem.h"
#include "Game.h"
#include "GameStateEditor.h"
#include <SDL.h>

namespace Temporal
{
	Keyboard::Keyboard()
	{
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
		_keysMap[SDLK_F12] = Key::F12;

		_keysMap[SDLK_GREATER] = Key::TILDE;
		_keysMap[SDLK_BACKQUOTE] = Key::TILDE;
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
		_keysMap[SDLK_QUOTE] = Key::QUOTE;

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
		//_keysMap[SDLK_LSUPER] = Key::WINDOWS;
		_keysMap[SDLK_LALT] = Key::LEFT_ALT;
		_keysMap[SDLK_SPACE] = Key::SPACE;
		_keysMap[SDLK_RALT] = Key::RIGHT_ALT;
		_keysMap[SDLK_RCTRL] = Key::RIGHT_CTRL;

		//_keysMap[SDLK_PRINT] = Key::PRINT_SCREEN;
		//_keysMap[SDLK_SCROLLOCK] = Key::SCROLL_LOCK;
		_keysMap[SDLK_PAUSE] = Key::PAUSE;
		_keysMap[SDLK_INSERT] = Key::INSERT;
		_keysMap[SDLK_HOME] = Key::HOME;
		_keysMap[SDLK_PAGEUP] = Key::PAGE_UP;
		_keysMap[SDLK_DELETE] = Key::DEL;
		_keysMap[SDLK_END] = Key::END;
		_keysMap[SDLK_PAGEDOWN] = Key::PAGE_DOWN;

		_keysMap[SDLK_LEFT] = Key::LEFT;
		_keysMap[SDLK_UP] = Key::UP;
		_keysMap[SDLK_DOWN] = Key::DOWN;
		_keysMap[SDLK_RIGHT] = Key::RIGHT;

		//_keysMap[SDLK_NUMLOCK] = Key::NUM_LOCK;
		_keysMap[SDLK_KP_DIVIDE] = Key::P_DIVIDE;
		_keysMap[SDLK_KP_MULTIPLY] = Key::P_MULTIPLY;
		_keysMap[SDLK_KP_MINUS] = Key::P_MINUS;
		_keysMap[SDLK_KP_PLUS] = Key::P_PLUS;
		_keysMap[SDLK_KP_ENTER] = Key::P_ENTER;
		_keysMap[SDLK_KP_PERIOD] = Key::P_PERIOD;
		_keysMap[SDLK_KP_7] = Key::P7;
		_keysMap[SDLK_KP_8] = Key::P8;
		_keysMap[SDLK_KP_9] = Key::P9;
		_keysMap[SDLK_KP_4] = Key::P4;
		_keysMap[SDLK_KP_5] = Key::P5;
		_keysMap[SDLK_KP_6] = Key::P6;
		_keysMap[SDLK_KP_1] = Key::P1;
		_keysMap[SDLK_KP_2] = Key::P2;
		_keysMap[SDLK_KP_3] = Key::P3;
		_keysMap[SDLK_KP_0] = Key::P0;

		for(IntKeyMapIterator i = _keysMap.begin(); i != _keysMap.end(); ++i)
		{
			_keys[i->second] = false;
		}
	}

	void Keyboard::raiseEvent(Message& message) const
	{
		Component* focusComponent = GameStateManager::get().getCurrentState().getEntitiesManager().getFocusInputComponent();
		if (focusComponent)
		{
			focusComponent->handleMessage(message);
		}
		else
		{
			ComponentList& components = GameStateManager::get().getCurrentState().getEntitiesManager().getInputComponents();
			// No iteration because we might add stuff to keyboard
			for (int i = 0; i < components.size(); ++i)
				components[i]->handleMessage(message);
		}
	}

	void Keyboard::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_QUIT)
		{
			Game::get().stop();
		}
		else if (e.type == SDL_KEYDOWN)
		{
			Key::Enum key = _keysMap[e.key.keysym.sym];
			if(!_keys[key])
			{
				_keys[key] = true;
				raiseEvent(Message(MessageID::KEY_DOWN, &key));
			}
			
		}
		else if (e.type == SDL_KEYUP)
		{
			Key::Enum key = _keysMap[e.key.keysym.sym];
			_keys[key] = false;
			raiseEvent(Message(MessageID::KEY_UP, &key));
			if(key == Key::ESC)
				Game::get().stop();
			if (key == Key::F12)
				GameStateEditor::loadEditor();
		}
	}
}
