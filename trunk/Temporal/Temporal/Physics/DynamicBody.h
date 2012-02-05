#pragma once

namespace Temporal { class Sensor; }

#include <Temporal\Base\Base.h>
#include "Body.h"
#include "Sensor.h"
#include <vector>

namespace Temporal
{
	class DynamicBody : public Body
	{
	public:
		DynamicBody(const Vector& size) : Body(size), _force(Vector::Zero), _gravityEnabled(true), _collision(Direction::NONE) {}
		virtual ~DynamicBody(void);

		virtual ComponentType::Enum getType(void) const { return ComponentType::DYNAMIC_BODY; }
		Orientation::Enum getOrientation(void) const;

		void applyForce(void);
		void applyGravity(void);
		void add(Sensor* const sensor);

		virtual void handleMessage(Message& message);
		void update(void);
		bool rayCast(const Vector& destination) const;

		void correctCollision(const Body& staticBody);
		void detectCollision(const Body& staticBody);

	private:
		Vector _force;
		bool _gravityEnabled;
		Direction::Enum _collision;
		std::vector<Sensor* const> _sensors;
	};
}