#ifndef ORIENTATION_H
#define ORIENTATION_H

#include <Temporal\Base\BaseEnums.h>
#include "Component.h"

namespace Temporal
{
	class Orientation : public Component
	{
	public:
		explicit Orientation(Side::Enum orientation) : _orientation(orientation) {}

		ComponentType::Enum getType(void) const { return ComponentType::ORIENTATION; }

		Side::Enum get(void) const { return _orientation; }

		void handleMessage(Message& message);
	private:
		Side::Enum _orientation;
	};
}
#endif