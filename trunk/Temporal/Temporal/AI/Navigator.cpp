#include "Navigator.h"
#include <Temporal\Base\Segment.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageUtils.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("STAT_NAV_WALK");	
		static const Hash FALL_STATE = Hash("STAT_NAV_FALL");
		static const Hash JUMP_UP_STATE = Hash("STAT_NAV_JUMP_UP");
		static const Hash JUMP_FORWARD_STATE = Hash("STAT_NAV_JUMP_FORWARD");
		static const Hash DESCEND_STATE = Hash("STAT_NAV_DESCEND");
		static const Hash WAIT_STATE = Hash("STAT_NAV_WAIT");
		static const Hash TURN_STATE = Hash("STAT_NAV_TURN");

		static const Hash ACTION_FALL_STATE = Hash("STAT_ACT_FALL");
		static const Hash ACTION_JUMP_END_STATE = Hash("STAT_ACT_JUMP_END");
		static const Hash ACTION_DROP_STATE = Hash("STAT_ACT_DROP");
		static const Hash ACTION_CLIMB_STATE = Hash("STAT_ACT_CLIMB");

		void Wait::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::SET_NAVIGATION_DESTINATION)
			{
				AABB goalPosition = *(const AABB*)message.getParam();
				Navigator& navigator = *(Navigator*)_stateMachine;

				AABB startPosition = AABB::Empty;
				_stateMachine->sendMessageToOwner(Message(MessageID::GET_BOUNDS, &startPosition));
				const NavigationNode* start = NavigationGraph::get().getNodeByAABB(startPosition);
				const NavigationNode* goal = NavigationGraph::get().getNodeByAABB(goalPosition);
				if(start != NULL && goal != NULL)
				{
					NavigationEdgeCollection* path = Pathfinder::get().findPath(start, goal);
					navigator.setDestination(goalPosition);
					navigator.setPath(path);
					_stateMachine->changeState(WALK_STATE);
				}
			}
		}

		void Walk::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::UPDATE)
			{
				const Point& position = *(Point*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_POSITION));
				float sourceX = position.getX();
				const Navigator& navigator = *((const Navigator*)_stateMachine);
				NavigationEdgeCollection* path = navigator.getPath();
				float targetX;
				bool reachedTargetPlatform;
				if(path == NULL)
				{
					const AABB& destination = navigator.getDestination();
					targetX = destination.getCenterX();
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
				if(abs(distance) <= 10.0f)
				{
					if(reachedTargetPlatform)
					{
						_stateMachine->changeState(WAIT_STATE);
					}
					else
					{
						_stateMachine->changeState(TURN_STATE);
						
						// Prevent stop when edge is walk
						_stateMachine->handleMessage(message);
					}
				}
				else
				{
					Orientation::Enum orientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
					if(distance < 0)
						sendDirectionAction(*_stateMachine, Orientation::LEFT);
					else
						sendDirectionAction(*_stateMachine, Orientation::RIGHT);
				}
			}
		}

		void Turn::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::UPDATE)
			{
				Navigator* navigator = (Navigator*)_stateMachine;
				NavigationEdgeCollection* path = navigator->getPath();
				const NavigationEdge* edge = (*path)[0];
				Orientation::Enum currentOrientation = *(Orientation::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
				Orientation::Enum targetOrientation = edge->getOrientation();
				if(currentOrientation != targetOrientation)
				{
					_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
				}
				else
				{
					path->erase(path->begin());
					if(path->size() == 0)
						navigator->setPath(NULL);
					if(edge->getType() == NavigationEdgeType::DESCEND)
						navigator->changeState(DESCEND_STATE);
					else if(edge->getType() == NavigationEdgeType::FALL)
						navigator->changeState(FALL_STATE);
					else if(edge->getType() == NavigationEdgeType::JUMP_FORWARD)
						navigator->changeState(JUMP_FORWARD_STATE);
					else if(edge->getType() == NavigationEdgeType::JUMP_UP)
						navigator->changeState(JUMP_UP_STATE);
					else if(edge->getType() == NavigationEdgeType::WALK)
					{
						navigator->changeState(WALK_STATE);
						// Prevent stop when edge is walk
						navigator->handleMessage(message);
					}
				}
			}
		}

		void Fall::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const Hash& state = *(Hash*)message.getParam();
				if(state == ACTION_FALL_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
			}
		}

		void JumpUp::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const Hash& state = *(Hash*)message.getParam();
				if(state == ACTION_CLIMB_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_UP));
			}
		}

		void JumpForward::enter(void) const
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_UP));
			_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
		}

		void JumpForward::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const Hash& state = *(Hash*)message.getParam();
				if(state == ACTION_JUMP_END_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
		}

		void Descend::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const Hash& state = *(Hash*)message.getParam();
				if(state == ACTION_DROP_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_DOWN));
			}
		}
	}

	using namespace NavigatorStates;
	StateCollection Navigator::getStates(void) const
	{
		StateCollection states;
		
		states[WALK_STATE] = new Walk();
		states[FALL_STATE] = new Fall();
		states[JUMP_UP_STATE] = new JumpUp();
		states[JUMP_FORWARD_STATE] = new JumpForward();
		states[DESCEND_STATE] = new Descend();
		states[WAIT_STATE] = new Wait();
		states[TURN_STATE] = new Turn();
		return states;
	}

	Hash Navigator::getInitialState(void) const
	{
		return WAIT_STATE;
	}

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			AABB position = AABB::Empty;
			sendMessageToOwner(Message(MessageID::GET_BOUNDS, &position));
			const NavigationNode* current = NavigationGraph::get().getNodeByAABB(position);
			NavigationEdgeCollection* path = getPath();
			if(current != NULL && path != NULL)
			{	
				for(NavigationEdgeIterator i = path->begin(); i != path->end(); ++i)
				{
					const NavigationEdge& edge = **i;
					const NavigationNode& next = edge.getTarget();
					Segment segment = Segment(current->getArea().getCenter(), next.getArea().getCenter());
					Graphics::get().draw(segment, Color::Cyan);
					current = &next;
				}
			}
		}
	}
}