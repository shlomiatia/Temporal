#pragma once

#include <Temporal\Base\Base.h>
#include "Component.h"

namespace Temporal
{
	class EntityOrientation : public Component
	{
	public:
		EntityOrientation(Orientation::Enum orientation) : _orientation(orientation) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::ORIENTATION; }

		virtual void handleMessage(Message& message);
	private:
		Orientation::Enum _orientation;

		EntityOrientation(const EntityOrientation&);
		EntityOrientation& operator=(const EntityOrientation&);
	};
}