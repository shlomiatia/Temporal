#ifndef LASER_H
#define LASER_H

#include "EntitySystem.h"
#include "Hash.h"
#include "BaseEnums.h"

namespace Temporal
{
	class OBB;

	class Laser : public Component
	{
	public:
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Laser(); };
		static const Hash TYPE;
	private:
		const OBB* _platform;

		void update(float framePeriod);

		friend class SerializationAccess;
	};
}

#endif