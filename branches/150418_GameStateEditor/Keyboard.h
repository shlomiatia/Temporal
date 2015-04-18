#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <unordered_map>

namespace Temporal
{
	namespace Key
	{
		enum Enum
		{
			NONE,
			ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,  
			BACKSPACE, CAPS_LOCK, ENTER, LEFT_SHIFT, RIGHT_SHIFT, LEFT_CTRL, WINDOWS, LEFT_ALT, SPACE, RIGHT_ALT, RIGHT_CTRL,

			PRINT_SCREEN, SCROLL_LOCK, PAUSE,
			INSERT, HOME, PAGE_UP,
			DEL, END, PAGE_DOWN,

			LEFT, UP, DOWN, RIGHT,

			NUM_LOCK, 

			TILDE = '~', D1 = '1', D2 = '2', D3 = '3', D4 = '4', D5 = '5', D6 = '6', D7 = '7', D8 = '8', D9 = '9', D0 = '0', MINUS = '_', PLUS = '+',
			TAB = '	', Q = 'Q', W = 'W', E = 'E', R = 'R', T = 'T', Y = 'Y', U = 'U', I = 'I', O = 'O', P = 'P', OPEN_BRACKET = '[', CLOSE_BRACKET = ']',
			A = 'A', S = 'S', D = 'D', F = 'F', G = 'G', H = 'H', J = 'J', K = 'K', L = 'L', SEMICOLON = ';', BACKSLASH = '\\', QUOTE = '\'',
			Z = 'Z', X = 'X', C = 'C', V = 'V', B = 'B', N = 'N', M = 'M', COMMA = ',', PERIOD = '.', SLASH = '/', ASTERISK = '*',

			SIZE
		};
	}

	class Message;
	class Component;
	typedef std::unordered_map<int, Key::Enum> IntKeyMap;
	typedef IntKeyMap::const_iterator IntKeyMapIterator;
	typedef std::unordered_map<Key::Enum, bool> KeyBoolMap;
	typedef KeyBoolMap::const_iterator KeyBoolMapIterator;


	class Keyboard
	{
	public:
		static Keyboard& get()
		{
			static Keyboard instance;
			return (instance);
		}

		void dispatchEvent(void* obj);

		bool getKey(Key::Enum key) const { return _keys.at(key); }

		void add(Component* component) { _components.push_back(component); }
		void clear() { _components.clear(); }
		void setFocus(Component* component) { _focus = component; }
		void clearFocus() { _focus = 0; }
	private:
		// TODO:
		IntKeyMap _keysMap;
		KeyBoolMap _keys;
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