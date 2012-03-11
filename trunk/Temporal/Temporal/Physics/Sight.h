#pragma once

#include <Temporal/Game/Component.h>
#include "Grid.h"

namespace Temporal
{
	// TODO: Ask for sight instead of getting it
	class Sight : public Component
	{
	public:
		Sight(float upperAngle, float lowerAngle) : _upperAngle(upperAngle), _lowerAngle(lowerAngle) {};

		virtual ComponentType::Enum getType(void) const { return ComponentType::SIGHT; }
		virtual void handleMessage(Message& message);

	private:
		const float _upperAngle;
		const float _lowerAngle;

		void checkLineOfSight(bool drawDebugInfo = false) const;
		void drawLineOfSight(const Vector &sourcePosition, Orientation::Enum sourceOrientation) const;

		bool rayCast(const Vector& source, const Vector& destination, bool drawDebugInfo) const;
	};
}