#ifndef DYNAMICBODY_H
#define DYNAMICBODY_H

#include <vector>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Rectangle.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class StaticBody;

	class DynamicBody : public Component
	{
	public:
		// BRODER
		static const Vector GRAVITY;

		explicit DynamicBody(const Size& size);

		ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }

		void handleMessage(Message& message);

	private:
		Size _size;
		Vector _absoluteImpulse;
		Vector _velocity;
		bool _gravityEnabled;
		Vector _collision;

		Rectangle getBounds(void) const;
		Orientation::Enum getOrientation(void) const;

		void changePosition(const Vector& offset);
		void update(float framePeriodInMillis);
		bool correctCollision(const StaticBody& staticBody);

		static bool correctCollision(void* caller, void* data, const StaticBody& staticBody);
	};
}
#endif