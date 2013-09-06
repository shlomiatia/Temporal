#include "Gamepad.h"
#include <SDL.h>

namespace Temporal
{
	void updateAxis(SDL_Event& e, int axis, float& value)
	{
		if(e.jaxis.axis == axis) 
		{
			if(abs(e.jaxis.value) > 16000)
			{
				value = e.jaxis.value;
			}
			else
			{
				value = 0.0f;
			}
		}
	}

	void updateAxis(SDL_Event& e, int axis, Vector& vector, Axis::Enum a)
	{
		float temp = -1.0f;
		updateAxis(e, axis, temp);
		if(temp != -1.0f)
			vector.setAxis(a, temp);
	}

	Gamepad::Gamepad() : _leftStick(Vector::Zero), _rightStick(Vector::Zero), _leftTrigger(0.0f), _rightTrigger(0.0f)
	{
		for(int i = 0; i < GamepadButton::SIZE; ++i)
			_buttons[i] = false;

		_buttonsMap[0] = GamepadButton::FRONT_DOWN;
		_buttonsMap[1] = GamepadButton::FRONT_RIGHT;
		_buttonsMap[2] = GamepadButton::FRONT_LEFT;
		_buttonsMap[3] = GamepadButton::FRONT_UP;
		_buttonsMap[4] = GamepadButton::BACK_LEFT;
		_buttonsMap[5] = GamepadButton::BACK_RIGHT;
		_buttonsMap[6] = GamepadButton::UTILITY_OTHER;
		_buttonsMap[7] = GamepadButton::UTILITY_START;
		_buttonsMap[8] = GamepadButton::STICK_LEFT;
		_buttonsMap[9] = GamepadButton::STICK_RIGHT;
	}

	void Gamepad::init()
	{
		_joystick = SDL_JoystickOpen(0);
	}

	void Gamepad::dispose()
	{
		SDL_JoystickClose(_joystick);
	}

	void Gamepad::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_JOYAXISMOTION)
		{
			updateAxis(e, 0, _leftStick, Axis::X);
			updateAxis(e, 1, _leftStick, Axis::Y);
			updateAxis(e, 3, _rightStick, Axis::X);
			updateAxis(e, 4, _rightStick, Axis::Y);
			float temp = -1.0f;
			updateAxis(e, 2, temp);
			if(temp != -1.0f)
			{
				if(temp < 0)
					_leftTrigger = temp;
				else
					_rightTrigger = temp;
			}
		}
		else if(e.type == SDL_JOYBUTTONDOWN)
		{
			_buttons[_buttonsMap[e.button.button]] = true;
		}
		else if(e.type == SDL_JOYBUTTONUP)
		{
			_buttons[_buttonsMap[e.button.button]] = false;
		}
		else if(e.type == SDL_JOYHATMOTION)
		{
			_buttons[GamepadButton::DPAD_UP] = e.jhat.value & SDL_HAT_UP;
			_buttons[GamepadButton::DPAD_DOWN] = e.jhat.value & SDL_HAT_DOWN;
			_buttons[GamepadButton::DPAD_LEFT] = e.jhat.value & SDL_HAT_LEFT;
			_buttons[GamepadButton::DPAD_RIGHT] = e.jhat.value & SDL_HAT_RIGHT;
		}
	}
}
