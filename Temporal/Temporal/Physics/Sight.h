#pragma once

#include <Temporal\Base\Vector.h>
#include <Temporal\Game\Component.h>
#include "Grid.h"

namespace Temporal
{
	// TODO: Ask for sight instead of getting it
	class Sight : public Component
	{
	public:
		Sight(float upperAngle, float lowerAngle) : _upperAngle(upperAngle), _lowerAngle(lowerAngle), _pointOfIntersection(Vector::Zero), _isSeeing(false) {};

		virtual ComponentType::Enum getType(void) const { return ComponentType::SIGHT; }
		virtual void handleMessage(Message& message);

	private:
		const float _upperAngle;
		const float _lowerAngle;

		Vector _pointOfIntersection;
		bool _isSeeing;

		void checkLineOfSight(void);
		void drawFieldOfView(const Vector &sourcePosition, Orientation::Enum sourceOrientation) const;
		void drawDebugInfo(void) const;

		bool rayCast(const Vector& source, const Vector& destination);
	};
}