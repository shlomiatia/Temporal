#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Input.h"
#include <unordered_map>

namespace Temporal
{
	namespace Key
	{
		enum Enum
		{
			NONE,
			ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			TILDE, D1, D2, D3, D4, D5, D6, D7, D8, D9, D0, MINUS, PLUS, BACKSPACE,
			TAB, Q, W, E, R, T, Y, U, I, O, P, OPEN_BRACKET, CLOSE_BRACKET, ENTER,
			CAPS_LOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, BACKSLASH,
			LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT,
			LEFT_CTRL, WINDOWS, LEFT_ALT, SPACE, RIGHT_ALT, RIGHT_CTRL,

			PRINT_SCREEN, SCROLL_LOCK, PAUSE,
			INSERT, HOME, PAGE_UP,
			DELETE, END, PAGE_DOWN,

			LEFT, UP, DOWN, RIGHT,

			NUM_LOCK, ASTERISK,

			SIZE
		};
	}

	class Message;
	class Component;
	typedef std::unordered_map<int, Key::Enum> KeyCollection;
	typedef KeyCollection::const_iterator KeyCollectionIterator;

	class Keyboard
	{
	public:
		static Keyboard& get()
		{
			static Keyboard instance;
			return (instance);
		}

		void dispatchEvent(void* obj);

		void add(Component* component) { _components.push_back(component); }
		void setFocus(Component* component) { _focus = component; }
		void clearFocus() { _focus = 0; }
	private:
		// TODO:
		KeyCollection _keysMap;
		std::vector<Component*> _components;
		Component* _focus;

		void raiseEvent(Message& message) const;

		Keyboard();
		~Keyboard() {};
		Keyboard(const Keyboard&);
		Keyboard& operator=(const Keyboard&);
	};
}
#endif