#include "Navigator.h"
#include "NavigatorWalk.h"
#include "NavigatorFall.h"
#include "NavigatorJumpUp.h"
#include "NavigatorJumpForward.h"
#include "NavigatorDescend.h"
#include "NavigatorWait.h"
#include "Serialization.h"
#include "MessageUtils.h"
#include "Graphics.h"
#include "StateMachineComponent.h"
#include "NavigationGraph.h"
#include "NavigationEdge.h"

namespace Temporal
{
	const Hash Navigator::TYPE = Hash("navigator");

	static const Hash WALK_STATE = Hash("NAV_STT_WALK");
	static const Hash FALL_STATE = Hash("NAV_STT_FALL");
	static const Hash JUMP_UP_STATE = Hash("NAV_STT_JUMP_UP");
	static const Hash JUMP_FORWARD_STATE = Hash("NAV_STT_JUMP_FORWARD");
	static const Hash DESCEND_STATE = Hash("NAV_STT_DESCEND");
	static const Hash WAIT_STATE = Hash("NAV_STT_WAIT");

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

	bool Navigator::plotPath(const Vector& goalPosition, Hash tracked)
	{
		int collistionGroup = *static_cast<int*>(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		const Vector& startPosition = getPosition(*this);

		// No shape because it's not ready on load
		const NavigationNode* start = getEntity().getManager().getGameState().getNavigationGraph().getNode(startPosition, collistionGroup);
		const NavigationNode* goal = getEntity().getManager().getGameState().getNavigationGraph().getNode(goalPosition, collistionGroup);
		bool success = false;
		if (start && goal)
		{
			NavigationEdgeList* path = Pathfinder::get().findPath(start, goal);
			if (path)
			{
				if (path->size() != 0)
				{
					setPath(path);
				}
				setDestination(goalPosition);
				if (tracked != Hash::INVALID)
					setTracked(tracked);
				changeState(WALK_STATE);
				success = true;
			}
		}
		if (!success)
		{
			raiseNavigationFailure();
		}
		return success;
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
					plotPath(destination);
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
				Graphics::get().getLinesSpriteBatch().add(obb, Color::Cyan);
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
				plotPath(_destination);
			}
		}
	}
}