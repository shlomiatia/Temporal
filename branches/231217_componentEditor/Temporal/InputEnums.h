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
			
			BACKQUOTE = '`', D1 = '1', D2 = '2', D3 = '3', D4 = '4', D5 = '5', D6 = '6', D7 = '7', D8 = '8', D9 = '9', D0 = '0', MINUS = '-', EQUALS = '=',
			TAB = '	', Q = 'q', W = 'w', E = 'e', R = 'r', T = 't', Y = 'y', U = 'u', I = 'i', O = 'o', P = 'p', OPEN_ANGLE_BRACKET = '[', CLOSE_ANGLE_BRACKET = ']',
			A = 'a', S = 's', D = 'd', F = 'f', G = 'g', H = 'h', J = 'j', K = 'k', L = 'l', SEMICOLON = ';', APOSTROPHE = '\'', BACKSLASH = '\\',
			Z = 'z', X = 'x', C = 'c', V = 'v', B = 'b', N = 'n', M = 'm', COMMA = ',', PERIOD = '.', SLASH = '/',

			TILDE = '~', EXCLAMATION_MARK = '!', AT = '@', HASH = '#', DOLLAR = '$', PERCENT = '%', CARET = '^', AMPERSAND = '&', ASTERISK = '*', OPEN_PARENTHESES = '(', CLOSE_PARENTHESES = ')', UNDERSCORE = '_', PLUS = '+',
			CQ = 'Q', CW = 'W', CE = 'E', CR = 'R', CT = 'T', CY = 'Y', CU = 'U', CI = 'I', CO = 'O', CP = 'P', OPEN_CURLY_BRACKET = '{', CLOSE_CURLY_BRACKET = '}',
			CA = 'A', CS = 'S', CD = 'D', CF = 'F', CG = 'G', CH = 'H', CJ = 'J', CK = 'K', CL = 'L', COLON = ':', QUOTATION = '"', PIPE = '|',
			CZ = 'Z', CX = 'X', CC = 'C', CV = 'V', CB = 'B', CN = 'N', CM = 'M', LESS_THEN = '<', GREATHER_THEN = '>', QUESTION_MARK = '?',

			DUMMY = 256,

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

			SIZE
		};
	}

}
#endif