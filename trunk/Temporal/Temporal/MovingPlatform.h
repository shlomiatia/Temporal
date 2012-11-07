#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class MovingPlatform : public Component
	{
	public:
		MovingPlatform(const Vector point1 = Vector::Zero, const Vector point2 = Vector::Zero)
			: _point1(point1), _point2(point2), _direction(false) {}

		ComponentType::Enum getType() const { return ComponentType::OTHER; }
		void handleMessage(Message& message);

		Component* clone() const { return new MovingPlatform(_point1, _point2); }

	private:
		Vector _point1;
		Vector _point2;
		bool _direction;

		friend class SerializationAccess;
	};
}

#endif