#ifndef STATICBODY_H
#define STATICBODY_H

#include "EntitySystem.h"

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

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const;
		static const Hash TYPE;
	private:

		Fixture* _fixture;

		friend class SerializationAccess;
	};
}
#endif