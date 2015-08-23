#ifndef INPUTENUMS_H
#define INPUTENUMS_H

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

			NUM_LOCK, P_DIVIDE, P_MULTIPLY, P_MINUS,
			P7, P8, P9, P_PLUS,
			P4, P5, P6,
			P1, P2, P3, P_ENTER,
			P0, P_PERIOD,


			TILDE = '~', D1 = '1', D2 = '2', D3 = '3', D4 = '4', D5 = '5', D6 = '6', D7 = '7', D8 = '8', D9 = '9', D0 = '0', MINUS = '-', PLUS = '+',
			TAB = '	', Q = 'Q', W = 'W', E = 'E', R = 'R', T = 'T', Y = 'Y', U = 'U', I = 'I', O = 'O', P = 'P', OPEN_BRACKET = '[', CLOSE_BRACKET = ']',
			A = 'A', S = 'S', D = 'D', F = 'F', G = 'G', H = 'H', J = 'J', K = 'K', L = 'L', SEMICOLON = ';', BACKSLASH = '\\', QUOTE = '\'',
			Z = 'Z', X = 'X', C = 'C', V = 'V', B = 'B', N = 'N', M = 'M', COMMA = ',', PERIOD = '.', SLASH = '/',

			SIZE
		};
	}

}
#endif