#include "Navigator.h"
#include "BaseUtils.h"
#include "Serialization.h"
#include "Segment.h"
#include "Message.h"
#include "MessageUtils.h"
#include "Graphics.h"

namespace Temporal
{
	namespace NavigatorStates
	{
		static const Hash WALK_STATE = Hash("NAV_STT_WALK");	
		static const Hash FALL_STATE = Hash("NAV_STT_FALL");
		static const Hash JUMP_UP_STATE = Hash("NAV_STT_JUMP_UP");
		static const Hash JUMP_FORWARD_STATE = Hash("NAV_STT_JUMP_FORWARD");
		static const Hash DESCEND_STATE = Hash("NAV_STT_DESCEND");
		static const Hash WAIT_STATE = Hash("NAV_STT_WAIT");
		static const Hash TURN_STATE = Hash("NAV_STT_TURN");

		static const Hash ACTION_FALL_STATE = Hash("ACT_STT_FALL");
		static const Hash ACTION_JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
		static const Hash ACTION_DROP_STATE = Hash("ACT_STT_DROP");
		static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");

		static const NumericPairSerializer DESTINATION_CENTER_SERIALIZER("NAV_SER_CENTER");
		static const NumericPairSerializer DESTINATION_RADIUS_SERIALIZER("NAV_SER_SIZE");

		void plotPath(StateMachineComponent& stateMachine, const AABB& goalPosition)
		{
			Navigator& navigator = (Navigator&)stateMachine;
			AABB startPosition = AABB::Zero;
			navigator.sendMessageToOwner(Message(MessageID::GET_BOUNDS, &startPosition));
			const NavigationNode* start = NavigationGraph::get().getNodeByAABB(startPosition);
			const NavigationNode* goal = NavigationGraph::get().getNodeByAABB(goalPosition);
			if(start != NULL && goal != NULL)
			{
				NavigationEdgeCollection* path = Pathfinder::get().findPath(start, goal);
				navigator.setDestination(goalPosition);
				navigator.setPath(path);
				navigator.changeState(WALK_STATE);
			}
		}

		void Wait::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::SET_NAVIGATION_DESTINATION)
			{
				AABB goalPosition = *(const AABB*)message.getParam();
				plotPath(*_stateMachine, goalPosition); 
			}
		}

		void Walk::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::UPDATE)
			{
				const Point& position = *(Point*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_POSITION));
				float sourceX = position.getX();
				Navigator& navigator = *((Navigator*)_stateMachine);
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
						navigator.setDestination(AABB::Zero);
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
					Side::Enum orientation = *(Side::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
					if(distance < 0)
						sendDirectionAction(*_stateMachine, Side::LEFT);
					else
						sendDirectionAction(*_stateMachine, Side::RIGHT);
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
				Side::Enum currentSide = *(Side::Enum*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
				Side::Enum targetSide = edge->getSide();
				if(currentSide != targetSide)
				{
					_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
				}
				else
				{
					path->erase(path->begin());
					if(path->size() == 0)
					{
						navigator->setPath(NULL);
					}
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

	void Navigator::deserialize(const Serialization& serialization)
	{
		if(_path != NULL)
		{
			_path->clear();
		}
		
		Point center = Point::Zero;
		DESTINATION_CENTER_SERIALIZER.deserialize(serialization, center);
		Vector radius = Vector::Zero;
		DESTINATION_RADIUS_SERIALIZER.deserialize(serialization, radius);
		AABB destination = AABB(center, radius);
			
		if(destination != AABB::Zero)
			plotPath(*this, destination);
	}

	void Navigator::debugDraw(void) const
	{
		Point currentPoint = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		NavigationEdgeCollection* path = getPath();
			
		if(path != NULL)
		{	
			for(NavigationEdgeIterator i = path->begin(); i != path->end(); ++i)
			{
				const NavigationEdge& edge = **i;
				const NavigationNode& next = edge.getTarget();
				Point nextPoint = next.getArea().getCenter();
				Segment segment = Segment(currentPoint, nextPoint);
				Graphics::get().draw(segment, Color::Cyan);
				currentPoint = nextPoint;
			}
		}
	}

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *(Serialization*)message.getParam();
			DESTINATION_CENTER_SERIALIZER.serialize(serialization, _destination.getCenter());
			DESTINATION_RADIUS_SERIALIZER.serialize(serialization, _destination.getRadius());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *(const Serialization*)message.getParam();
			deserialize(serialization);
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			debugDraw();
		}
	}
}