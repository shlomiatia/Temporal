#pragma once

#include <Temporal\Base\Base.h>
#include "Component.h"

namespace Temporal
{
	class Transform : public Component
	{
	public:
		Transform(const Vector& position, Orientation::Type orientation = Orientation::NONE) : _position(position), _orientation(orientation) {}

		virtual ComponentType::Type getType(void) const { return ComponentType::TRANSFORM; }
		virtual void handleMessage(Message& message);
	private:
		Vector _position;
		Orientation::Type _orientation;

		Transform(const Transform&);
		Transform& operator=(const Transform&);
	};
}