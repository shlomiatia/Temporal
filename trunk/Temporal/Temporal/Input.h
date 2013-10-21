#ifndef INPUT_H
#define INPUT_H

#include "Gamepad.h"

namespace Temporal
{
	namespace ButtonState
	{
		enum Enum
		{
			NONE,
			DOWN,
			UP
		};
	}

	class Input
	{
	public:
		static Input& get()
		{
			static Input instance;
			return (instance);
		}

		void init();
		void dispose();

		const Gamepad& getGamepad() const { return _gamepad; }

		void update();

	private:
		Gamepad _gamepad;

		Input() {}
		~Input() {}
		Input(const Input&);
		Input& operator=(const Input&);
	};
}
#endif