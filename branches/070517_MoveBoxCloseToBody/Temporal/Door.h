#ifndef DOOR_H
#define DOOR_H

#include "EntitySystem.h"

namespace Temporal
{
	class Door : public Component
	{
	public:
		Door(bool closed = true) { _closed = true; }

		Hash getType() const { return TYPE; }
		Component* clone() const { return new Door(_closed); }
		void handleMessage(Message& message);

		bool isClosed() const { return _closed; }
		void setClosed(bool closed) { _closed = closed; set(); }

		static const Hash TYPE;

	private:
		bool _closed;
		void set();

		friend class SerializationAccess;
	};
}

#endif