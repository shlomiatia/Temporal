#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "Vector.h"
#include "EntitySystem.h"

namespace Temporal
{
	class MovingPlatform : public Component
	{
	public:
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new MovingPlatform(); }

		static const Hash TYPE;
	private:

		friend class SerializationAccess;
	};
}

#endif