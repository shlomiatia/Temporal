#ifndef INPUT_H
#define INPUT_H

#include "Vector.h"
#include <unordered_map>

namespace Temporal
{
	namespace Key
	{
		enum Enum
		{
			ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			TILDE, D1, D2, D3, D4, D5, D6, D7, D8, D9, D0, MINUS, PLUS, BACKSPACE,
			TAB, Q, W, E, R, T, Y, U, I, O, P, OPEN_BRACKET, CLOSE_BRACKET, ENTER,
			CAPS_LOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, BACKSLASH,
			LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT,
			LEFT_CTRL, LEFT_ALT, SPACE, RIGHT_ALT, RIGHT_CTRL,

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

	class Input
	{
	public:
		static Input& get()
		{
			static Input instance;
			return (instance);
		}

		void update();

		bool quit() const { return _quit; }
		bool key(Key::Enum key) const { return _keys[key]; }
		const Vector& mouse() const { return _mouse; }

	private:
		KeyCollection _keysMap;
		bool _keys[Key::SIZE];
		bool _quit;
		Vector _mouse;

		Input();
		~Input() {}
		Input(const Input&);
		Input& operator=(const Input&);
	};
}
#endif