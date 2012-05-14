#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include <Temporal\Base\BaseEnums.h>

namespace Temporal
{
	class Component;
	class Message;
	class Hash;

	void sendDirectionAction(const Component& component, Side::Enum direction);
	bool isSensorCollisionMessage(Message& message, const Hash& sensorID);
}
#endif