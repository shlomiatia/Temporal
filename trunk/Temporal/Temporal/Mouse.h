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
			NONE,
			LEFT,
			MIDDLE,
			RIGHT,
			WHEEL_DOWN,
			WHEEL_UP,

			SIZE
		};
	}

	typedef std::unordered_map<int, MouseButton::Enum> IntMouseButtonMap;
	typedef IntMouseButtonMap::const_iterator IntMouseButtonIterator;

	typedef std::unordered_map<MouseButton::Enum, bool> MouseButtonBoolMap;
	typedef MouseButtonBoolMap::const_iterator MouseButtonBoolIterator;

	class Mouse
	{
	public:
		static Mouse& get()
		{
			static Mouse instance;
			return (instance);
		}

		void dispatchEvent(void* obj);

		bool getButton(MouseButton::Enum button) const { return _buttons.at(button); }
		const Vector& getPosition() const { return _position; }

	private:
		// TODO:
		IntMouseButtonMap _buttonsMap;
		MouseButtonBoolMap _buttons;
		Vector _position;


		Mouse();
		~Mouse() {};
		Mouse(const Mouse&);
		Mouse& operator=(const Mouse&);
	};
}
#endif