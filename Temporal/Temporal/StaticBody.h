#ifndef STATICBODY_H
#define STATICBODY_H

#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class Fixture;

	class StaticBody : public Component
	{
	public:
		StaticBody(Fixture* fixture = 0)  : _fixture(fixture) {}
		~StaticBody();

		Fixture& getFixture() { return *_fixture; }
		const Fixture& getFixture() const { return *_fixture; }

		Hash getType() const { return ComponentsIds::STATIC_BODY; }
		void handleMessage(Message& message);

		Component* clone() const;
		
	private:

		Fixture* _fixture;

		friend class SerializationAccess;
	};
}
#endif