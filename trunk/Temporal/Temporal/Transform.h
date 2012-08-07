#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "NumericPair.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Transform : public Component
	{
	public:
		explicit Transform(const Point& position, const Side::Enum orientation = Side::RIGHT) : _position(position), _orientation(orientation) {}

		const Point& getPosition() const { return _position; }
		Side::Enum getOrientation() const { return _orientation; }

		ComponentType::Enum getType() const { return ComponentType::TRANSFORM; }
		void handleMessage(Message& message);
		Component* clone() const;
	private:
		Point _position;
		Side::Enum _orientation;
	};
}
#endif