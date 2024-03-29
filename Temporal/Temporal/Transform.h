#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector.h"
#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class Transform : public Component
	{
	public:
		explicit Transform(const Vector& position = Vector::Zero, const Side::Enum orientation = Side::RIGHT) : _position(position), _orientation(orientation) {}

		const Vector& getPosition() const { return _position; }
		Side::Enum getOrientation() const { return _orientation; }

		Hash getType() const { return ComponentsIds::TRANSFORM; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Transform(_position, _orientation); }
		
	private:

		Vector _position;
		Side::Enum _orientation;

		friend class SerializationAccess;
	};
}
#endif