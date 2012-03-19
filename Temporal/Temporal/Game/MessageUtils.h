#pragma once

#include <Temporal\Base\Enums.h>

namespace Temporal
{
	class Component;

	void sendDirectionAction(const Component& component, Orientation::Enum direction);
}