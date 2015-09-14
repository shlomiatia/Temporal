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
		Laser(bool friendly = false, float speedPerSecond = 128.0f) : _platform(0), _friendly(friendly), _speedPerSecond(speedPerSecond) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Laser(_friendly, _speedPerSecond); };
		static const Hash TYPE;

		bool isFriendly() const { return _friendly; }
		void setFriendly(bool friendly) { _friendly = friendly; }
		float getSpeedPerSecond() const { return _speedPerSecond; }
		void setSpeedPerSecond(float speedPerSecond) { _speedPerSecond = speedPerSecond; }

	private:
		const OBB* _platform;
		bool _friendly;
		float _speedPerSecond;

		void update(float framePeriod);
		void setLength(float length);
		void setColor();

		friend class SerializationAccess;
	};
}

#endif