#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "InputEnums.h"
#include <unordered_map>

namespace Temporal
{
	class Message;
	typedef std::unordered_map<int, Key::Enum> IntKeyMap;
	typedef IntKeyMap::const_iterator IntKeyMapIterator;
	typedef std::unordered_map<Key::Enum, bool> KeyBoolMap;
	typedef KeyBoolMap::const_iterator KeyBoolMapIterator;
	typedef std::unordered_map<Key::Enum, Key::Enum> KeyKeyMap;
	typedef KeyKeyMap::const_iterator KeyKeyMapIterator;

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
	private:
		IntKeyMap _keysMap;
		KeyBoolMap _keys;
		KeyKeyMap _shiftKeys;

		void raiseEvent(Message& message) const;

		Keyboard();
		~Keyboard() {};
		Keyboard(const Keyboard&);
		Keyboard& operator=(const Keyboard&);
	};
}
#endif