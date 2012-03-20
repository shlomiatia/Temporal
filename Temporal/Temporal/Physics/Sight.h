#ifndef SIGHT_H
#define SIGHT_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\Vector.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
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
#endif