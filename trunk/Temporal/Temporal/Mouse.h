#ifndef MOUSE_H
#define MOUSE_H

#include "Vector.h"
#include "Input.h"
#include "EntitySystem.h"
#include "Delegate.h"
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

	class MouseParams;

	class MouseListener : public Component
	{
	public:
		MouseListener() : _isLeftDown(false), _isLeftClick(false), _isRightDown(false), _isRightClick(false), _leftMouseDownEvent(0),
			_leftMouseClickEvent(0), _leftMouseUpEvent(0), _rightMouseDownEvent(0), _rightMouseClickEvent(0), _rightMouseUpEvent(0), _mouseMoveEvent(0) {}
		~MouseListener();

		virtual void handleMessage(Message& message);

		void setLeftMouseDownEvent(IAction1<const MouseParams&>* leftMouseDownEvent) { setEvent(_leftMouseDownEvent, leftMouseDownEvent); }
		void setLeftMouseClickEvent(IAction1<const MouseParams&>* leftMouseClickEvent) { setEvent(_leftMouseClickEvent, leftMouseClickEvent); }
		void setLeftMouseUpEvent(IAction1<const MouseParams&>* leftMouseUpEvent) { setEvent(_leftMouseUpEvent, leftMouseUpEvent); }
		void setRightMouseDownEvent(IAction1<const MouseParams&>* rightMouseDownEvent) { setEvent(_rightMouseDownEvent, rightMouseDownEvent); }
		void setRightMouseClickEvent(IAction1<const MouseParams&>* rightMouseClickEvent) { setEvent(_rightMouseClickEvent, rightMouseClickEvent); }
		void setRightMouseUpEvent(IAction1<const MouseParams&>* rightMouseUpEvent) { setEvent(_rightMouseUpEvent, rightMouseUpEvent); }
		void setMouseMoveEvent(IAction1<const MouseParams&>* mouseMoveEvent) { setEvent(_mouseMoveEvent, mouseMoveEvent); }

	private:
		static const Hash TYPE;

		IAction1<const MouseParams&>* _leftMouseDownEvent;
		IAction1<const MouseParams&>* _leftMouseClickEvent;
		IAction1<const MouseParams&>* _leftMouseUpEvent;
		IAction1<const MouseParams&>* _rightMouseDownEvent;
		IAction1<const MouseParams&>* _rightMouseClickEvent;
		IAction1<const MouseParams&>* _rightMouseUpEvent;
		IAction1<const MouseParams&>* _mouseMoveEvent;

		bool _isLeftDown;
		bool _isLeftClick;
		bool _isRightDown;
		bool _isRightClick;

		void setEvent(IAction1<const MouseParams&>*& prop, IAction1<const MouseParams&>* value);
	};
}
#endif