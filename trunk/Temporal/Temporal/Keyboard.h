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
		// TODO:
		IntKeyMap _keysMap;
		KeyBoolMap _keys;

		void raiseEvent(Message& message) const;

		Keyboard();
		~Keyboard() {};
		Keyboard(const Keyboard&);
		Keyboard& operator=(const Keyboard&);
	};
}
#endif