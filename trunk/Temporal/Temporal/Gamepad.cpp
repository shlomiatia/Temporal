#include "Gamepad.h"
#include "GameState.h"
#include "EntitySystem.h"
#include <SDL.h>

namespace Temporal
{
	void updateAxis(SDL_Event& e, int axis, float& value)
	{
		if(e.caxis.axis == axis) 
		{
			if(abs(e.caxis.value) > 16000)
			{
				value = e.caxis.value;
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

		_buttonsMap[SDL_CONTROLLER_BUTTON_A] = GamepadButton::ACTION_DOWN;
		_buttonsMap[SDL_CONTROLLER_BUTTON_B] = GamepadButton::ACTION_RIGHT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_X] = GamepadButton::ACTION_LEFT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_Y] = GamepadButton::ACTION_UP;
		_buttonsMap[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = GamepadButton::SHOULDER_LEFT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = GamepadButton::SHOULDER_RIGHT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_BACK] = GamepadButton::UTILITY_OTHER;
		_buttonsMap[SDL_CONTROLLER_BUTTON_START] = GamepadButton::UTILITY_START;
		_buttonsMap[SDL_CONTROLLER_BUTTON_LEFTSTICK] = GamepadButton::STICK_LEFT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = GamepadButton::STICK_RIGHT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = GamepadButton::DPAD_DOWN;
		_buttonsMap[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = GamepadButton::DPAD_RIGHT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = GamepadButton::DPAD_LEFT;
		_buttonsMap[SDL_CONTROLLER_BUTTON_DPAD_UP] = GamepadButton::DPAD_UP;
	}

	void Gamepad::init()
	{
		_gamepad = SDL_GameControllerOpen(0);
	}

	void Gamepad::dispose()
	{
		SDL_GameControllerClose(static_cast<SDL_GameController*>(_gamepad));
	}

	void Gamepad::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_CONTROLLERAXISMOTION)
		{
			updateAxis(e, SDL_CONTROLLER_AXIS_LEFTX, _leftStick, Axis::X);
			updateAxis(e, SDL_CONTROLLER_AXIS_LEFTY, _leftStick, Axis::Y);
			updateAxis(e, SDL_CONTROLLER_AXIS_RIGHTX, _rightStick, Axis::X);
			updateAxis(e, SDL_CONTROLLER_AXIS_RIGHTY, _rightStick, Axis::Y);
			updateAxis(e, SDL_CONTROLLER_AXIS_TRIGGERLEFT, _leftTrigger);
			updateAxis(e, SDL_CONTROLLER_AXIS_TRIGGERLEFT, _rightTrigger);
		}
		else if(e.type == SDL_CONTROLLERBUTTONDOWN)
		{
			GamepadButton::Enum button = _buttonsMap[e.cbutton.button];
			if(!_buttons[button])
			{
				_buttons[button] = true;
				GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::GAMEPAD_BUTTON_DOWN, &button));
			}
		}
		else if(e.type == SDL_CONTROLLERBUTTONUP)
		{
			GamepadButton::Enum button = _buttonsMap[e.cbutton.button];
			GameStateManager::get().getCurrentState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::GAMEPAD_BUTTON_UP, &button));
			_buttons[button] = false;
		}
	}
}
