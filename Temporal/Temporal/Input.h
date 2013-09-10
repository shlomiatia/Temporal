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
			START_PUSHING,
			PUSHING,
			STOP_PUSHING
		};

		inline bool isPushing(ButtonState::Enum state)
		{
			return state == ButtonState::START_PUSHING || state == ButtonState::PUSHING;
		}
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