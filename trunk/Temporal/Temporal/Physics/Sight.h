#pragma once

#include <Temporal/Game/Component.h>

namespace Temporal
{
	// TODO: Who're you watching?
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
		bool rayCast(const Vector& source, const Vector& destination, bool drawDebugInfo) const;

		// TODO: Remove copy constructor/validate virtual destructors
		Sight(const Sight&);
		Sight& operator=(const Sight&);
	};
}