#include "NavigatorWalk.h"
#include "Navigator.h""
#include "MessageUtils.h"
#include "NavigationEdge.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");
		static const Hash FALL_STATE = Hash("NAV_STT_FALL");
		static const Hash JUMP_UP_STATE = Hash("NAV_STT_JUMP_UP");
		static const Hash JUMP_FORWARD_STATE = Hash("NAV_STT_JUMP_FORWARD");
		static const Hash DESCEND_STATE = Hash("NAV_STT_DESCEND");

		void Walk::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				update();
			}
		}

		void Walk::update()
		{
			Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
			Hash tracked = navigator.getTracked();
			Vector goalPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(tracked, Message(MessageID::GET_POSITION)));
			const Vector& destination = navigator.getDestination();
			NavigationEdgeList* path = navigator.getPath();

			if (goalPosition != destination || (path &&  (*path)[0]->getTarget().isDisabled()))
			{
				navigator.plotPath(goalPosition);
				return;
			}

			const Vector& position = getPosition(*_stateMachine);
			float sourceX = position.getX();
			
			
			float targetX;
			bool reachedTargetPlatform;
			if (!path)
			{
				targetX = destination.getX();
				reachedTargetPlatform = true;
			}
			else
			{
				const NavigationEdge* edge = (*path)[0];
				targetX = edge->getX();
				reachedTargetPlatform = false;
			}

			float distance = targetX - sourceX;

			// BRODER
			if (abs(distance) > 10.0f)
			{
				Side::Enum orientation = getOrientation(*_stateMachine);
				if (distance < 0)
				{
					sendDirectionAction(*_stateMachine, Side::LEFT);
				}
				else
				{
					sendDirectionAction(*_stateMachine, Side::RIGHT);
				}
					
			}
			else
			{
				if (!reachedTargetPlatform)
				{
					updateNext();
				}
				else
				{
					navigator.raiseNavigationSuccess();
				}
			}
		}

		void Walk::updateNext()
		{
			Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
			NavigationEdgeList* path = navigator.getPath();
			const NavigationEdge* edge = (*path)[0];
			Side::Enum currentSide = getOrientation(*_stateMachine);
			Side::Enum targetSide = edge->getSide();
			if (targetSide != 0 && currentSide != targetSide)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_BACKWARD));
			}

			path->erase(path->begin());
			if (path->size() == 0)
			{
				navigator.setPath(0);
			}
			if (edge->getType() == NavigationEdgeType::DESCEND)
				navigator.changeState(DESCEND_STATE);
			else if (edge->getType() == NavigationEdgeType::JUMP_FORWARD)
				navigator.changeState(JUMP_FORWARD_STATE);
			else if (edge->getType() == NavigationEdgeType::JUMP_UP)
				navigator.changeState(JUMP_UP_STATE);
			else if (edge->getType() == NavigationEdgeType::FALL)
				navigator.changeState(FALL_STATE);
			else if (edge->getType() == NavigationEdgeType::WALK)
				navigator.changeState(WALK_STATE);
			else if (edge->getType() == NavigationEdgeType::TEMPORAL_TRAVEL)
			{
				int targetCollisionGroup = getIntParam(_stateMachine->getEntity().getManager().sendMessageToEntity(navigator.getTracked(), Message(MessageID::GET_COLLISION_GROUP)));
				_stateMachine->raiseMessage(Message(MessageID::SET_TEMPORAL_PERIOD, &targetCollisionGroup));
				navigator.changeState(WALK_STATE);
			}
				
		}
	}
}