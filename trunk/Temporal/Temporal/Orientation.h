#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "BaseEnums.h"
#include "Component.h"

namespace Temporal
{
	class Orientation : public Component
	{
	public:
		explicit Orientation(Side::Enum orientation) : _orientation(orientation) {}

		ComponentType::Enum getType(void) const { return ComponentType::ORIENTATION; }
		void handleMessage(Message& message);
		Component* clone(void) const;
	private:
		Side::Enum _orientation;
	};
}
#endif