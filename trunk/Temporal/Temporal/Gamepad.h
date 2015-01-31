#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "Vector.h"
#include <unordered_map>

namespace Temporal
{
	namespace GamepadButton
	{
		enum Enum
		{
			ACTION_LEFT, ACTION_RIGHT, ACTION_UP, ACTION_DOWN,

			SHOULDER_LEFT, SHOULDER_RIGHT,

			STICK_LEFT, STICK_RIGHT,

			DPAD_UP, DPAD_DOWN, DPAD_LEFT, DPAD_RIGHT,

			UTILITY_START, UTILITY_OTHER,

			SIZE
		};
	}

	typedef std::unordered_map<int, GamepadButton::Enum> GamepadButtonCollection;
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
		void* _gamepad;

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