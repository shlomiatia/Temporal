#ifndef STATICBODY_H
#define STATICBODY_H

#include "Body.h"

namespace Temporal
{
	class StaticBody : public Body
	{
	public:
		StaticBody(const Size& size, bool cover) : Body(size), _cover(cover) {};

		bool isCover(void) const { return _cover; }
		virtual ComponentType::Enum getType(void) const { return ComponentType::STATIC_BODY; }
		virtual void handleMessage(Message& message);

	private:
		bool _cover;
	};
}
#endif