#ifndef MOUSE_H
#define MOUSE_H

#include "Vector.h"
#include <unordered_map>

namespace Temporal
{
	namespace MouseButton
	{
		enum Enum
		{
			LEFT,
			MIDDLE,
			RIGHT,

			SIZE
		};
	}

	typedef std::unordered_map<int, int> MouseButtonCollection;
	typedef MouseButtonCollection::const_iterator MouseButtonIterator;

	class Mouse
	{
	public:
		static Mouse& get()
		{
			static Mouse instance;
			return (instance);
		}

		void update(void* obj);

		const Vector& getPosition() const { return _position; }
		bool getButton(MouseButton::Enum button) { return _buttons[button]; }

	private:
		Vector _position;
		MouseButtonCollection _buttonsMap;
		bool _buttons[MouseButton::SIZE];

		Mouse();
		~Mouse() {};
		Mouse(const Mouse&);
		Mouse& operator=(const Mouse&);
	};
}
#endif