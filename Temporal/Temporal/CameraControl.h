#pragma once
#include "StateMachineComponent.h"
#include "Ids.h"

namespace Temporal
{
	class CameraControl : public Component
	{
	public:

		Hash getType() const { return ComponentsIds::CAMERA_CONTROL; }
		void handleMessage(Message& message);

		Component* clone() const { return new CameraControl(); }

	};
}
