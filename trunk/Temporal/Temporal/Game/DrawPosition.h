#pragma once

#include <Temporal\Base\Base.h>
#include "Component.h"

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

		DrawPosition(const DrawPosition&);
		DrawPosition& operator=(const DrawPosition&);
	};
}