#ifndef INPUT_H
#define INPUT_H

#include "Gamepad.h"

namespace Temporal
{
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