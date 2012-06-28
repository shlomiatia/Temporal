#ifndef POSITION_H
#define POSITION_H

#include "NumericPair.h"
#include "Component.h"

namespace Temporal
{
	class Position : public Component
	{
	public:
		explicit Position(const Point& position) : _position(position) {}

		ComponentType::Enum getType(void) const { return ComponentType::POSITION; }
		void handleMessage(Message& message);
		Component* clone(void) const;
	private:
		Point _position;
	};
}
#endif