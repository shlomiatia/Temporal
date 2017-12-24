#ifndef LASER_H
#define LASER_H

#include "EntitySystem.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "Ids.h"

namespace Temporal
{
	class OBB;

	class Laser : public Component
	{
	public:
		Laser(bool friendly = false, float speedPerSecond = 128.0f) : _platform(0), _friendly(friendly), _speedPerSecond(speedPerSecond) {}

		Hash getType() const { return ComponentsIds::LASER; }
		void handleMessage(Message& message);
		Component* clone() const { return new Laser(_friendly, _speedPerSecond); };

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