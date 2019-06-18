#include "NavigatorWalk.h"
#include "Navigator.h""
#include "MessageUtils.h"
#include "NavigationEdge.h"
#include "NavigationGraph.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");
		static const Hash FALL_STATE = Hash("NAV_STT_FALL");
		static const Hash JUMP_UP_STATE = Hash("NAV_STT_JUMP_UP");
		static const Hash JUMP_FORWARD_STATE = Hash("NAV_STT_JUMP_FORWARD");
		static const Hash DESCEND_STATE = Hash("NAV_STT_DESCEND");
		static const Hash SKIP_BOX_STATE = Hash("NAV_STT_SKIP_BOX");

		void Walk::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				update();
			}
			else if (message.getID() == MessageID::COLLISIONS_CORRECTED)
			{
				const Vector& collision = getVectorParam(message.getParam());
				if (collision.getX() != 0.0f && collision.getY() == 0.0f)
				{
					_stateMachine->changeState(SKIP_BOX_STATE);
				}
			}
		}

		void Walk::update()
		{
			Navigator& navigator = static_cast<Navigator&>(*_stateMachine);
			Hash tracked = navigator.getTracked();
			Vector goalPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(tracked, Message(MessageID::GET_POSITION)));
			const Vector& destination = navigator.getDestination();
			NavigationEdgeList* path = navigator.getPath();
			bool isEmptyPath = !path || (path->size() == 0);

			int targetCollistionGroup = *static_cast<int*>(navigator.getEntity().getManager().sendMessageToEntity(navigator.getTracked(), Message(MessageID::GET_COLLISION_GROUP)));
			const NavigationNode* start = navigator.getEntity().getManager().getGameState().getNavigationGraph().getNode(goalPosition, targetCollistionGroup);
			if ((goalPosition != destination && start) || (!isEmptyPath && path->at(0)->getTarget().isDisabled()))
			{
				navigator.plotPath();
				return;
			}

			const Vector& position = getPosition(*_stateMachine);
			float sourceX = position.getX();
			
			
			float targetX;
			bool reachedTargetPlatform;
			if (isEmptyPath)
			{
				targetX = destination.getX();
				reachedTargetPlatform = true;
			}
			else
			{
				const NavigationEdge* edge = path->at(0);
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