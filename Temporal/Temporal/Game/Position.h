#pragma once

#include <Temporal\Base\Base.h>
#include "Component.h"

namespace Temporal
{
	class Position : public Component
	{
	public:
		Position(const Vector& position) : _position(position) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::POSITION; }
		virtual void handleMessage(Message& message);
	private:
		Vector _position;
	};
}