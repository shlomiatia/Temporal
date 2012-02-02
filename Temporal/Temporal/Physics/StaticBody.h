#pragma once

#include "Body.h"

namespace Temporal
{
	class StaticBody : public Body
	{
	public:
		StaticBody(const Vector& size) : Body(size) {};

		virtual ComponentType::Type getType(void) const { return ComponentType::STATIC_BODY; }
	};
}