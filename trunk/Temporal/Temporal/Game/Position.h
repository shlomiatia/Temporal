#ifndef POSITION_H
#define POSITION_H

#include <Temporal\Base\NumericPair.h>
#include "Component.h"

namespace Temporal
{
	class Position : public Component
	{
	public:
		explicit Position(const Point& position) : _position(position) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::POSITION; }
		virtual void handleMessage(Message& message);
	private:
		Point _position;
	};
}
#endif