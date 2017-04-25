#ifndef TEMPORALNOTIFICATION_H
#define TEMPORALNOTIFICATION_H

#include "EntitySystem.h"

namespace Temporal
{
	class TemporalNotification : public Component
	{
	public:
		TemporalNotification() : Component(true) {}

		void handleMessage(Message& message);
		Hash getType() const { return Hash::INVALID; }
		Component* clone() const { return 0; }
	};
}

#endif