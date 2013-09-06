#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "Vector.h"
#include <unordered_map>
#include <SDL.h>

namespace Temporal
{
	namespace GamepadButton
	{
		enum Enum
		{
			FRONT_LEFT, FRONT_RIGHT, FRONT_UP, FRONT_DOWN,

			BACK_LEFT, BACK_RIGHT,

			STICK_LEFT, STICK_RIGHT,

			DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT,

			UTILITY_START, UTILITY_OTHER,

			SIZE
		};
	}

	typedef std::unordered_map<int, int> GamepadButtonCollection;
	typedef GamepadButtonCollection::const_iterator GamepadButtonIterator;

	class Gamepad
	{
	public:
		Gamepad();
		~Gamepad() {};

		void init();
		void dispose();
		void dispatchEvent(void* obj);

		const Vector& getLeftStick() const { return _leftStick; }
		const Vector& getRightStick() const { return _rightStick; }
		float getLeftTrigger() const { return _leftTrigger; }
		float getRightTrigger() const { return _rightTrigger; }
		bool getButton(GamepadButton::Enum button) const { return _buttons[button]; }

	private:

		// TODO:
		GamepadButtonCollection _buttonsMap;
		SDL_Joystick* _joystick;

		bool _buttons[GamepadButton::SIZE];
		Vector _leftStick;
		Vector _rightStick;
		float _leftTrigger;
		float _rightTrigger;

		Gamepad(const Gamepad&);
		Gamepad& operator=(const Gamepad&);
	};
}
#endif