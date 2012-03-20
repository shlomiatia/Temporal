#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include <Temporal\Base\BaseEnums.h>

namespace Temporal
{
	class Component;

	void sendDirectionAction(const Component& component, Orientation::Enum direction);
}
#endif