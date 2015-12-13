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

		static const Hash ACTION_JUMP_END_STATE = Hash("ACT_STT_JUMP_END");
		static const Hash ACTION_JUMP_STATE = Hash("ACT_STT_JUMP");
		static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");
		static const Hash ACTION_WALK_STATE = Hash("ACT_STT_WALK");

		static const Hash TIME_MACHINE_ID = Hash("ENT_TIME_MACHINE0");

		Navigator& getNavigator(StateMachineComponent& stateMachine)
		{
			return static_cast<Navigator&>(stateMachine);
		}

		bool plotPath(StateMachineComponent& stateMachine, const Vector& goalPosition, Hash tracked = Hash::INVALID)
		{
			int collistionGroup = *static_cast<int*>(stateMachine.raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
			Navigator& navigator = getNavigator(stateMachine);
			const Vector& startPosition = getPosition(stateMachine);

			// No shape because it's not ready on load
			const NavigationNode* start = stateMachine.getEntity().getManager().getGameState().getNavigationGraph().getNode(startPosition, collistionGroup);
			const NavigationNode* goal = stateMachine.getEntity().getManager().getGameState().getNavigationGraph().getNode(goalPosition, collistionGroup);
			bool success = false;
			if (start && goal)
			{
				NavigationEdgeList* path = Pathfinder::get().findPath(start, goal);
				if (path)
				{
					if (path->size() != 0)
					{
						navigator.setPath(path);
					}
					navigator.setDestination(goalPosition);
					if (tracked != Hash::INVALID)
						navigator.setTracked(tracked);
					navigator.changeState(WALK_STATE);
					success = true;
				}
			}
			if (!success)
			{
				getNavigator(stateMachine).raiseNavigationFailure();
			}
			return success;
		}

		void Wait::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::NAVIGATE)
			{
				Hash tracked = *static_cast<const Hash*>(message.getParam());
				int sourceCollisionGroup = getIntParam(_stateMachine->raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
				int targetCollisionGroup = getIntParam(_stateMachine->getEntity().getManager().sendMessageToEntity(tracked, Message(MessageID::GET_COLLISION_GROUP)));
				Hash destinationId = tracked;
				if (sourceCollisionGroup != targetCollisionGroup)
				{
					destinationId = TIME_MACHINE_ID;
					getNavigator(*_stateMachine).setTimeMachine(true);
				}
					
				const Vector& goalPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(destinationId, Message(MessageID::GET_POSITION)));
				plotPath(*_stateMachine, goalPosition, tracked);
			}
		}

		void Walk::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::UPDATE)
			{
				update();
			}
		}

		void Walk::update()
		{
			const Vector& position = getPosition(*_stateMachine);
			float sourceX = position.getX();
			Navigator& navigator = getNavigator(*_stateMachine);
			NavigationEdgeList* path = navigator.getPath();
			float targetX;
			bool reachedTargetPlatform;
			if (!path)
			{
				const Vector& destination = navigator.getDestination();
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
					sendDirectionAction(*_stateMachine, Side::LEFT);
				else
					sendDirectionAction(*_stateMachine, Side::RIGHT);
			}
			else
			{
				if (!reachedTargetPlatform)
				{
					updateNext();
				}
				else
				{
					if (!navigator.isTimeMachine())
					{
						navigator.raiseNavigationSuccess();
					}
					else
					{
						int targetCollisionGroup = getIntParam(_stateMachine->getEntity().getManager().sendMessageToEntity(navigator.getTracked(), Message(MessageID::GET_COLLISION_GROUP)));
						_stateMachine->raiseMessage(Message(MessageID::SET_TEMPORAL_PERIOD, &targetCollisionGroup));
						navigator.setTimeMachine(false);
						Hash tracked = navigator.getTracked();
						Vector goalPosition = getVectorParam(_stateMachine->getEntity().getManager().sendMessageToEntity(tracked, Message(MessageID::GET_POSITION)));
						Hash temporalFutureId = getHashParam(_stateMachine->raiseMessage(Message(MessageID::GET_TEMPORAL_FUTURE_ID)));
						if (temporalFutureId != Hash::INVALID)
						{
							_stateMachine->getEntity().getManager().sendMessageToEntity(temporalFutureId, Message(MessageID::SET_POSITION, &goalPosition));
						}
						
						plotPath(*_stateMachine, goalPosition);
					}
					
					
				}
				
			}
			
		}

		void Walk::updateNext()
		{
			Navigator& navigator = getNavigator(*_stateMachine);
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

		}

		void Fall::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
			_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
		}

		void Fall::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_JUMP_END_STATE)
					_stateMachine->changeState(_afterLoad ? WAIT_STATE : WALK_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
			}
		}

		void JumpUp::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
		}

		void JumpUp::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_CLIMB_STATE)
					_stateMachine->changeState(_afterLoad ? WAIT_STATE : WALK_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_START));
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
			}
		}

		void JumpForward::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
			if (!_afterLoad)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_FORWARD));
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_START));
			}

		}

		void JumpForward::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_ENTERED)
			{
				Hash state = getHashParam(message.getParam());
				if (state != ACTION_CLIMB_STATE && state != ACTION_JUMP_STATE && state != JUMP_FORWARD_STATE && state != ACTION_WALK_STATE)
					_stateMachine->changeState(_afterLoad ? WAIT_STATE : WALK_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
			}
		}

		void Descend::enter(void* param)
		{
			_afterLoad = param && getBoolParam(param);
		}

		void Descend::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::STATE_EXITED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_JUMP_END_STATE)
					_stateMachine->changeState(_afterLoad ? WAIT_STATE : WALK_STATE);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				_stateMachine->raiseMessage(Message(MessageID::ACTION_DOWN));
			}
		}
	}

	using namespace NavigatorStates;
	HashStateMap Navigator::getStates() const
	{
		HashStateMap states;
		
		states[WALK_STATE] = new Walk();
		states[FALL_STATE] = new Fall();
		states[JUMP_UP_STATE] = new JumpUp();
		states[JUMP_FORWARD_STATE] = new JumpForward();
		states[DESCEND_STATE] = new Descend();
		states[WAIT_STATE] = new Wait();
		return states;
	}

	Hash Navigator::getInitialState() const
	{
		return WAIT_STATE;
	}

	void Navigator::raiseNavigationSuccess()
	{
		Hash id = getTracked();
		raiseMessage(Message(MessageID::NAVIGATION_SUCCESS, &id));
		setDestination(Vector::Zero);
		setTracked(Hash::INVALID);
		changeState(WAIT_STATE);
	}

	void Navigator::raiseNavigationFailure()
	{
		raiseMessage(Message(MessageID::NAVIGATION_FAILURE));
		setDestination(Vector::Zero);
		setTracked(Hash::INVALID);
		changeState(WAIT_STATE);
		
	}

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if (message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if (message.getID() == MessageID::DRAW_DEBUG)
		{
			debugDraw();
		}
		else if (message.getID() == MessageID::POST_LOAD)
		{
			postLoad();
		}
		else if (message.getID() == MessageID::STOP_NAVIGATE)
		{
			raiseNavigationFailure();
		}
	}

	void Navigator::update()
	{
		if (getCurrentStateID() == WALK_STATE && _destination != Vector::Zero)
		{
			if (!getEntity().getManager().getEntity(_tracked))
			{
				raiseNavigationFailure();
			}
			else if (!_timeMachine)
			{
				const Vector& destination = getVectorParam(getEntity().getManager().sendMessageToEntity(_tracked, Message(MessageID::GET_POSITION)));
				if (_destination != destination)
				{
					plotPath(*this, destination);
				}
			}
		}
	}

	void Navigator::debugDraw() const
	{
		Vector currentPoint = getPosition(*this);
		NavigationEdgeList* path = getPath();

		if (path)
		{
			for (NavigationEdgeIterator i = path->begin(); i != path->end(); ++i)
			{
				const NavigationEdge& edge = **i;
				const NavigationNode& next = edge.getTarget();
				Vector nextPoint = next.getArea().getCenter();
				Segment segment = SegmentPP(currentPoint, nextPoint);
				Vector segmentRadius = segment.getRadius();
				Vector axis = segmentRadius.normalize();
				Vector radius = Vector(segmentRadius.getLength(), 1.0);
				OBB obb = OBB(segment.getCenter(), axis, radius);
				Graphics::get().getSpriteBatch().add(obb, Color::Cyan);
				currentPoint = nextPoint;
			}
		}
	}

	void Navigator::postLoad()
	{
		setPath(0);
		if (getCurrentStateID() == DESCEND_STATE)
		{
			bool afterLoad = true;
			changeState(DESCEND_STATE, &afterLoad);
		}
		else if (getCurrentStateID() == JUMP_FORWARD_STATE)
		{
			bool afterLoad = true;
			changeState(JUMP_FORWARD_STATE, &afterLoad);
		}
		else if (getCurrentStateID() == JUMP_UP_STATE)
		{
			bool afterLoad = true;
			changeState(JUMP_UP_STATE, &afterLoad);
		}
		else if (getCurrentStateID() == FALL_STATE)
		{
			bool afterLoad = true;
			changeState(FALL_STATE, &afterLoad);
		}
		else
		{
			if (_destination != Vector::Zero)
			{
				plotPath(*this, _destination);
			}
		}
	}
}