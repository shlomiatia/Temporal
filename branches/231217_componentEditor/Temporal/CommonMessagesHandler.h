#ifndef COMMONMESSAGESHANDLER_H
#define COMMONMESSAGESHANDLER_H

#include "StateMachineComponent.h"
#include "MessageUtils.h"
#include "Vector.h"

namespace Temporal
{
	class SensorParams;

	class CommonMessagesHandler
	{
	public:
		CommonMessagesHandler(StateMachineComponent& controller) : _controller(controller), _isDescending(false), _isActivating(false), _isTakingDown(false) {}

		bool handleStandWalkAimMessage(Message& message);
		bool handleFallJumpMessage(Message& message);

	private:
		bool _isDescending;
		bool _isActivating;
		bool _isTakingDown;

		bool handleSensorSense(Message& message);
		bool activate(SensorParams& params);
		bool takedown(SensorParams& params);

		StateMachineComponent& _controller;
	};
}

#endif