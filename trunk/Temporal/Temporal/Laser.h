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
		Laser(bool activated = true) : _platform(0), _activated(activated){}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Laser(_activated); };
		static const Hash TYPE;
	private:
		const OBB* _platform;
		bool _activated;

		void update(float framePeriod);
		void setLength(float length);

		friend class SerializationAccess;
	};
}

#endif