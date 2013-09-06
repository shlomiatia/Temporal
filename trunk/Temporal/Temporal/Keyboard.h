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

	typedef std::unordered_map<int, int> KeyCollection;
	typedef KeyCollection::const_iterator KeyCollectionIterator;


	namespace KeyboardEvent
	{
		enum Enum
		{
			NONE,
			DOWN,
			PRESSSED,
			UP
		};
	}

	class Keyboard
	{
	public:
		static Keyboard& get()
		{
			static Keyboard instance;
			return (instance);
		}

		void update();
		void dispatchEvent(void* obj);
		bool isKeyPressed(Key::Enum key) { KeyboardEvent::Enum status = _keys[key];  return status == KeyboardEvent::PRESSSED || status == KeyboardEvent::DOWN; }
		bool isKeyDown(Key::Enum key) { return _keys[key] == KeyboardEvent::DOWN; }
		bool isKeyUp(Key::Enum key) { return _keys[key] == KeyboardEvent::UP; }


	private:

		// TODO:
		KeyCollection _keysMap;
		KeyboardEvent::Enum _keys[Key::SIZE];

		Keyboard();
		~Keyboard() {};
		Keyboard(const Keyboard&);
		Keyboard& operator=(const Keyboard&);
	};
}
#endif