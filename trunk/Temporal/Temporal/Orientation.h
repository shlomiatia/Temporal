#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "BaseEnums.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Orientation : public Component
	{
	public:
		explicit Orientation(Side::Enum orientation) : _orientation(orientation) {}

		ComponentType::Enum getType() const { return ComponentType::ORIENTATION; }
		void handleMessage(Message& message);
		Component* clone() const;
	private:
		Side::Enum _orientation;
	};
}
#endif