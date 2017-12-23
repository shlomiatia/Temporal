#ifndef ACTIVATION_SENSOR_H
#define ACTIVATION_SENSOR_H

#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class ActivationSensor : public Component
	{
	public:
		ActivationSensor(const Hash& target = Hash::INVALID) : _target(target), _shouldActivate(false){}

		Hash getType() const { return ComponentsIds::ACTIVATION_SENSOR; }
		void handleMessage(Message& message);

		Component* clone() const { return new ActivationSensor(_target); }
		void setTarget(Hash target) { _target = target; }
		Hash getTarget() const { return _target; }

	private:
		Hash _target;
		bool _shouldActivate;

		friend class SerializationAccess;
	};
}

#endif