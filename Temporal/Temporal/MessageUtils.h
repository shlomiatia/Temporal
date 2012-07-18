#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include "BaseEnums.h"

namespace Temporal
{
	class Component;
	class Message;
	class Hash;

	void sendDirectionAction(const Component& component, Side::Enum direction);
	bool isSensorCollisionMessage(Message& message, const Hash& sensorID);
	int getPeriod(const Component& component);
	int getPlayerPeriod(void);
}
#endif