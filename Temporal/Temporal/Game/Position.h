#ifndef POSITION_H
#define POSITION_H

#include <Temporal\Base\Vector.h>
#include "Component.h"

namespace Temporal
{
	class Position : public Component
	{
	public:
		Position(const Vector& position) : _position(position) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::POSITION; }
		virtual void handleMessage(Message& message);
	private:
		Vector _position;
	};
}
#endif