#ifndef DRAWPOSITION_H
#define DRAWPOSITION_H

#include "Component.h"
#include <Temporal\Base\Vector.h>

namespace Temporal
{
	class DrawPosition : public Component
	{
	public:
		DrawPosition(const Vector& offset) : _offset(offset), _override(Vector::Zero) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::DRAW_POSITION; }
		virtual void handleMessage(Message& message);
	private:
		Vector _offset;
		Vector _override;
	};
}
#endif