#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Rect.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class StaticBody;

	class DynamicBody : public Component
	{
	public:
		// BRODER
		static const float GRAVITY;

		explicit DynamicBody(const Size& size);

		ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }

		void handleMessage(Message& message);

	private:
		Size _size;
		Vector _velocity;
		Vector _force;
		Vector _impulse;
		bool _gravityEnabled;
		Direction::Enum _collision;

		Rect getBounds(void) const;

		Orientation::Enum getOrientation(void) const;
		void update(float framePeriodInMillis);
		void applyVelocity() const;
		void determineVelocity(float framePeriodInMillis);
		void handleCollisions(void);
		bool correctCollision(const StaticBody& staticBody);
		static bool correctCollision(void* caller, void* data, const StaticBody& staticBody);
	};
}
#endif