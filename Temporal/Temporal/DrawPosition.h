#ifndef DRAWPOSITION_H
#define DRAWPOSITION_H

#include "Component.h"
#include "NumericPair.h"

namespace Temporal
{
	class DrawPosition : public Component
	{
	public:
		explicit DrawPosition(const Vector& offset) : _offset(offset), _override(Vector::Zero) {}

		ComponentType::Enum getType(void) const { return ComponentType::DRAW_POSITION; }
		void handleMessage(Message& message);
		Component* clone(void) const;
	private:
		const Vector _offset;

		Point _override;
	};
}
#endif