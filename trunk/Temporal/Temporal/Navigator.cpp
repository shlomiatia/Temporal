#include "Navigator.h"
#include "Serialization.h"
#include "MessageUtils.h"
#include "Graphics.h"

namespace Temporal
{
	const Hash Navigator::TYPE = Hash("navigator");

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
		static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");

		Navigator& getNavigator(StateMachineComponent& stateMachine)
		{
			return static_cast<Navigator&>(stateMachine);
		}

		void plotPath(StateMachineComponent& stateMachine, const OBB& goalPosition)
		{
			Navigator& navigator = getNavigator(stateMachine);
			const OBBAABBWrapper& startPosition = *static_cast<OBBAABBWrapper*>(navigator.raiseMessage(Message(MessageID::GET_SHAPE)));
			
			const NavigationNode* start = stateMachine.getEntity().getManager().getGameState().getNavigationGraph().getNode(startPosition.getOBB());
			const NavigationNode* goal = stateMachine.getEntity().getManager().getGameState().getNavigationGraph().getNode(goalPosition);
			if(start && goal)
			{
				NavigationEdgeCollection* path = Pathfinder::get().findPath(start, goal);
				if(path)
				{
					if(path->size() != 0)
					{
						navigator.setPath(path);
					}
					navigator.setDestination(goalPosition);
					navigator.changeState(WALK_STATE);
				}
			}
		}

		void Wait::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::SET_NAVIGATION_DESTINATION)
			{
				const OBB& goalPosition = *static_cast<const OBB*>(message.getParam());
				plotPath(*_stateMachine, goalPosition); 
			}
		}

		void Walk::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::UPDATE)
			{
				const Vector& position = getPosition(*_stateMachine);
				float sourceX = position.getX();
				Navigator& navigator = getNavigator(*_stateMachine);
				NavigationEdgeCollection* path = navigator.getPath();
				float targetX;
				bool reachedTargetPlatform;
				if(!path)
				{
					const OBB& destination = navigator.getDestination();
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
						navigator.setDestination(OBB::Zero);
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
					Side::Enum orientation = getOrientation(*_stateMachine);
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
				Navigator& navigator = getNavigator(*_stateMachine);
				NavigationEdgeCollection* path = navigator.getPath();
				const NavigationEdge* edge = (*path)[0];
				Side::Enum currentSide = getOrientation(*_stateMachine);
				Side::Enum targetSide = edge->getSide();
				if(currentSide != targetSide)
				{
					_stateMachine->raiseMessage(Message(MessageID::ACTION_BACKWARD));
				}
				else
				{
					path->erase(path->begin());
					if(path->size() == 0)
					{
						navigator.setPath(0);
					}
					if(edge->getType() == NavigationEdgeType::DESCEND)
						navigator.changeState(DESCEND_STATE);
					else if(edge->getType() == NavigationEdgeType::FALL)
						navigator.changeState(FALL_STATE);
					else if(edge->getType() == NavigationEdgeType::JUMP_FORWARD)
						navigator.changeState(JUMP_FORWARD_STATE);
					else if(edge->getType() == NavigationEdgeType::JUMP_UP)
						navigator.changeState(JUMP_UP_STATE);
					else if(edge->getType() == NavigationEdgeType::WALK)
					{
						navigator.changeState(WALK_STATE);
						// Prevent stop when edge is walk
						navigator.handleMessage(message);
					}
				}
			}
		}

		void Fall::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if(state == ACTION_FALL_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			}
		}

		void JumpUp::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if(state == ACTION_CLIMB_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP));
			}
		}

		void JumpForward::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			_stateMachine->raiseMessage(Message(MessageID::ACTION_UP));
		}

		void JumpForward::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if(state == ACTION_JUMP_END_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP));
			}
		}

		void Descend::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::STATE_ENTERED)
			{
				Hash state = getHashParam(message.getParam());
				if(state == ACTION_FALL_STATE)
					_stateMachine->changeState(WALK_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_DOWN));
			}
		}
	}

	using namespace NavigatorStates;
	StateCollection Navigator::getStates() const
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

	Hash Navigator::getInitialState() const
	{
		return WAIT_STATE;
	}

	/*void Navigator::deserialize(const Serialization& serialization)
	{
		if(_path)
		{
			_path->clear();
		}
		
		Vector center = Vector::Zero;
		DESTINATION_CENTER_SERIALIZER.deserialize(serialization, center);
		Vector radius = Vector::Zero;
		DESTINATION_RADIUS_SERIALIZER.deserialize(serialization, radius);
		AABB destination = AABB(center, radius);
			
		if(destination != AABB::Zero)
			plotPath(*this, destination);
	}*/

	void Navigator::debugDraw() const
	{
		Vector currentPoint = getPosition(*this);
		NavigationEdgeCollection* path = getPath();
			
		if(path)
		{	
			for(NavigationEdgeIterator i = path->begin(); i != path->end(); ++i)
			{
				const NavigationEdge& edge = **i;
				const NavigationNode& next = edge.getTarget();
				Vector nextPoint = next.getArea().getCenter();
				Segment segment = SegmentPP(currentPoint, nextPoint);
				Vector segmentRadius = segment.getRadius();
				Vector axis = segmentRadius.normalize();
				Vector radius  = Vector(segmentRadius.getLength(), 1.0);
				OBB obb = OBB(segment.getCenter(), axis, radius);
				Graphics::get().getSpriteBatch().add(obb, Color::Cyan);
				currentPoint = nextPoint;
			}
		}
	}

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		/*if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			DESTINATION_CENTER_SERIALIZER.serialize(serialization, _destination.getCenter());
			DESTINATION_RADIUS_SERIALIZER.serialize(serialization, _destination.getRadius());
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			deserialize(serialization);
		}
		else*/ if(message.getID() == MessageID::DRAW_DEBUG)
		{
			debugDraw();
		}
	}
}