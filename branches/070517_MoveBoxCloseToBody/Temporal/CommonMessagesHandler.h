#ifndef COMMONMESSAGESHANDLER_H
#define COMMONMESSAGESHANDLER_H

#include "StateMachineComponent.h"
#include "Sensor.h"
#include "MessageUtils.h"

namespace Temporal
{
	class CommonMessagesHandler
	{
	public:
		CommonMessagesHandler(StateMachineComponent& controller) : _controller(controller), _isDescending(false), _isActivating(false), _isTakingDown(false) {}

		bool handleStandWalkAimMessage(Message& message);
		bool handleFallJumpMessage(Message& message);
		bool handleDragMessage(Message& message);
		bool handleDragWalkMessage(Message& message);
		void handleDragEnter();
		void handleDragWalkEnter();
		void setDraggableId(Hash draggableId) { _draggableId = draggableId; }

	private:
		bool _isDescending;
		bool _isActivating;
		bool _isTakingDown;
		Hash _draggableId;

		bool handleSensorSense(Message& message);
		bool activate(SensorParams& params);
		bool takedown(SensorParams& params);
		Vector getMovement();
		void move(Vector& movement);
		void moveDraggable(Vector& movement);

		StateMachineComponent& _controller;
	};
}

#endif