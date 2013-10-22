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

	typedef std::unordered_map<int, MouseButton::Enum> MouseButtonCollection;
	typedef MouseButtonCollection::const_iterator MouseButtonIterator;

	class Mouse
	{
	public:
		static Mouse& get()
		{
			static Mouse instance;
			return (instance);
		}

		void dispatchEvent(void* obj);

	private:
		MouseButtonCollection _buttonsMap;

		Mouse();
		~Mouse() {};
		Mouse(const Mouse&);
		Mouse& operator=(const Mouse&);
	};

	//class Action;
	class Component;
	class Message;

	class MouseListener
	{
	public:
		MouseListener(Component& owner) : _owner(owner), _isClick(false), _isDown(false) {}
		~MouseListener();

		void handleMessage(Message& message);

		virtual void mouseDown(MouseButton::Enum button) {}
		virtual void mouseClick(MouseButton::Enum button) {}
		virtual void mouseUp(MouseButton::Enum button) {}
	private:
		Component& _owner;
/*		Action* MouseDownEvent;
		Action* MouseClickEvent;
		Action* MouseUpEvent;*/

		bool _isDown;
		bool _isClick;
	};
}
#endif