#ifndef STATICBODY_H
#define STATICBODY_H

#include "EntitySystem.h"

namespace Temporal
{
	class Fixture;

	class StaticBody : public Component
	{
	public:
		StaticBody(Fixture* fixture);
		~StaticBody();

		Fixture& getFixture() { return *_fixture; }

		ComponentType::Enum getType() const { return ComponentType::STATIC_BODY; }
		void handleMessage(Message& message);

	private:
		Fixture* _fixture;
	};
}
#endif