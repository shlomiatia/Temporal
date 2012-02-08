#pragma once

#include "Body.h"

namespace Temporal
{
	class StaticBody : public Body
	{
	public:
		StaticBody(const Vector& size, bool cover) : Body(size), _cover(cover) {};

		bool isCover(void) const { return _cover; }
		virtual ComponentType::Enum getType(void) const { return ComponentType::STATIC_BODY; }

	private:
		bool _cover;
	};
}