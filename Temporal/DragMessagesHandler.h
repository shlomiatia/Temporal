#ifndef DRAGMESSAGESHANDLER_H
#define DRAGMESSAGESHANDLER_H

#include "StateMachineComponent.h"
#include "Vector.h"

namespace Temporal
{
	class DragMessagesHandler
	{
	public:
		DragMessagesHandler(StateMachineComponent& controller) : _controller(controller) {}

		bool handleDragMessage(Message& message);
		bool handleDragWalkMessage(Message& message);
		void handleDragEnter();
		void setDraggableId(Hash draggableId);

	private:
		Hash _draggableId;
		Vector _distance;

		bool checkIfStillWalking();
		bool handleDragWalkUpdate(Message& message);
		bool handleDistanceChange(Vector originalDraggablePosition);
		Vector getMovement();
		void move(Vector movement, float framePeriod);
		void moveDraggable(Vector movement, float framePeriod);

		StateMachineComponent& _controller;
	};
}

#endif