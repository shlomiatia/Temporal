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
		// TODO:
		MouseButtonCollection _buttonsMap;

		Mouse();
		~Mouse() {};
		Mouse(const Mouse&);
		Mouse& operator=(const Mouse&);
	};
}
#endif