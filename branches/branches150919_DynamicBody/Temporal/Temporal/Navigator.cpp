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

		static const Hash ACTION_FALL_STATE = Hash("ACT_STT_FALL");
		static const Hash ACTION_JUMP_STATE = Hash("ACT_STT_JUMP");
		static const Hash ACTION_CLIMB_STATE = Hash("ACT_STT_CLIMB");
		static const Hash ACTION_WALK_STATE = Hash("ACT_STT_WALK");

		Navigator& getNavigator(StateMachineComponent& stateMachine)
		{
			return static_cast<Navigator&>(stateMachine);
		}

		bool plotPath(StateMachineComponent& stateMachine, const OBB& goalPosition)
		{
			int collistionGroup = *static_cast<int*>(stateMachine.raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
			Navigator& navigator = getNavigator(stateMachine);
			OBB startPosition = OBBAABB(getPosition(stateMachine), Vector(1.0f, 1.0f));

			// No shape because it's not ready on load
			const NavigationNode* start = stateMachine.getEntity().getManager().getGameState().getNavigationGraph().getNode(startPosition, collistionGroup);
			const NavigationNode* goal = stateMachine.getEntity().getManager().getGameState().getNavigationGraph().getNode(goalPosition, collistionGroup);
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
					navigator.changeState(WALK_STATE);
					return true;
				}
			}
			return false;
		}

		void Wait::handleMessage(Message& message)
		{
			if (message.getID() == MessageID::SET_NAVIGATION_DESTINATION)
			{
				const OBB& goalPosition = *static_cast<const OBB*>(message.getParam());
				plotPath(*_stateMachine, goalPosition);
			}
			else if (message.getID() == MessageID::UPDATE)
			{
				const OBB& destination = getNavigator(*_stateMachine).getDestination();
				if (destination != OBB::Zero)
					plotPath(*_stateMachine, destination);
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
			if (abs(distance) <= 10.0f)
			{
				if (reachedTargetPlatform)
				{
					navigator.setDestination(OBB::Zero);
					_stateMachine->changeState(WAIT_STATE);
				}
				else
				{
					updateNext();
				}
			}
			else
			{
				Side::Enum orientation = getOrientation(*_stateMachine);
				if (distance < 0)
					sendDirectionAction(*_stateMachine, Side::LEFT);
				else
					sendDirectionAction(*_stateMachine, Side::RIGHT);
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
				if (state == ACTION_FALL_STATE)
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
			if (message.getID() == MessageID::STATE_ENTERED)
			{
				Hash state = getHashParam(message.getParam());
				if (state == ACTION_FALL_STATE)
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

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if (message.getID() == MessageID::DRAW_DEBUG)
		{
			debugDraw();
		}
		else if (message.getID() == MessageID::POST_LOAD)
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
				if (_destination != OBB::Zero)
				{
					if (!plotPath(*this, _destination))
					{
						changeState(WAIT_STATE);
					}
				}
			}
		}
	}
}