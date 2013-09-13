#ifndef MOUSE_H
#define MOUSE_H

#include "Vector.h"
#include "Input.h"
#include <unordered_map>

namespace Temporal
{
	namespace MouseButton
	{
		enum Enum
		{
			LEFT,
			MIDDLE,
			RIGHT,
			WHEEL_DOWN,
			WHEEL_UP,

			SIZE
		};
	}

	typedef std::unordered_map<int, int> MouseButtonCollection;
	typedef MouseButtonCollection::const_iterator MouseButtonIterator;

	class Mouse
	{
	public:
		static Mouse& get()
		{
			static Mouse instance;
			return (instance);
		}

		void update();
		void dispatchEvent(void* obj);

		const Vector& getPosition() const { return _position; }
		bool isClicking(MouseButton::Enum key) { return ButtonState::isPushing(_buttons[key]); }
		bool isStartClicking(MouseButton::Enum key) { return _buttons[key] == ButtonState::START_PUSHING; }
		bool isStopClicking(MouseButton::Enum key) { return _buttons[key] == ButtonState::STOP_PUSHING; }

	private:
		Vector _position;
		MouseButtonCollection _buttonsMap;
		ButtonState::Enum _buttons[MouseButton::SIZE];

		Mouse();
		~Mouse() {};
		Mouse(const Mouse&);
		Mouse& operator=(const Mouse&);
	};
}
#endif