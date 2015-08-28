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
		Laser(bool friendly = false) : _platform(0), _friendly(friendly){}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Laser(_friendly); };
		static const Hash TYPE;

		bool isFriendly() const { return _friendly; }
		void setFriendly(bool friendly) { _friendly = friendly; }
	private:
		const OBB* _platform;
		bool _friendly;

		void update(float framePeriod);
		void setLength(float length);
		void setColor();

		friend class SerializationAccess;
	};
}

#endif