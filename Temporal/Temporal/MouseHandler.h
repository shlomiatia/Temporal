#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include "EntitySystem.h"

namespace Temporal
{
	class MouseHandler : public Component
	{
	public:
		MouseHandler() : _isClick(false), _isDown(false) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }

		Component* clone() const { return new MouseHandler(); }

		static const Hash TYPE;
	private:
		bool _isDown;
		bool _isClick;
	};
}

#endif