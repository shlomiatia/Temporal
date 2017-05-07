#ifndef MOUSE_H
#define MOUSE_H

#include "Vector.h"
#include "Input.h"
#include "GameEnums.h"
#include "InputEnums.h"
#include <unordered_map>

namespace Temporal
{
	class Message;
	class MouseParams;

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
		Vector getOffsetPosition() const;

	private:
		// TODO:
		IntMouseButtonMap _buttonsMap;
		MouseButtonBoolMap _buttons;
		Vector _position;

		void sendMessage(MessageID::Enum messageId, MouseParams& params);

		Mouse();
		~Mouse() {};
		Mouse(const Mouse&);
		Mouse& operator=(const Mouse&);
	};
}
#endif