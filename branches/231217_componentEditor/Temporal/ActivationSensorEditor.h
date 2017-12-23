#ifndef ACTIVATION_SENSOR_EDITOR_H
#define ACTIVATION_SENSOR_EDITOR_H

#include "ComponentEditor.h"

namespace Temporal
{
	class ActivationSensor;

	class ActivationSensorEditor : public ComponentEditor
	{
	public:
		ActivationSensorEditor(ActivationSensor& activationSensor) : _activationSensor(activationSensor) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		ActivationSensor& _activationSensor;

		void targetChanged(const char* s);
	};
}

#endif