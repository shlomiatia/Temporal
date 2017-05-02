#ifndef COMMONMESSAGESHANDLER_H
#define COMMONMESSAGESHANDLER_H

#include "StateMachineComponent.h"
#include "Sensor.h"

namespace Temporal
{
	class CommonMessagesHandler
	{
	public:
		CommonMessagesHandler(StateMachineComponent& controller) : _controller(controller), _isDescending(false), _isActivating(false), _isTakingDown(false) {}

		bool handleStandWalkMessage(Message& message);
		bool handleFallJumpMessage(Message& message);
		bool handleDragMessage(Message& message);
		void handleMessage(Message& message);
		void handleDragEnter();
		void die();

	private:
		bool _isDescending;
		bool _isActivating;
		bool _isTakingDown;

		StateMachineComponent& _controller;
	};
}

#endif