#include "Navigator.h"
#include <Temporal\Base\Segment.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageUtils.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	namespace NavigatorStates
	{
		void Wait::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::SET_NAVIGATION_DESTINATION)
			{
				const Point* goalPosition = (const Point*)message.getParam();
				Navigator& navigator = *(Navigator*)_stateMachine;

				const Point& startPosition = *(Point*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_POSITION));
				const NavigationNode* start = NavigationGraph::get().getNodeByPosition(startPosition);
				const NavigationNode* goal = NavigationGraph::get().getNodeByPosition(*goalPosition);
				if(start != NULL && goal != NULL)
				{
					NavigationEdgeCollection* path = Pathfinder::get().findPath(start, goal);
					navigator.setDestination(goalPosition);
					navigator.setPath(path);
					_stateMachine->changeState(NavigatorStates::WALK);
				}
			}
		}

		void Walk::handleMessage(Message& message)
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
					const Point& destination = navigator.getDestination();
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
				if(abs(distance) <= 10.0f)
				{
					if(reachedTargetPlatform)
						_stateMachine->changeState(NavigatorStates::WAIT);
					else
						_stateMachine->changeState(NavigatorStates::TURN);
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

		void Turn::handleMessage(Message& message)
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
					if(edge->getType() == NavigationEdgeType::FALL)
						_stateMachine->changeState(NavigatorStates::FALL);
					else if(edge->getType() == NavigationEdgeType::JUMP)
						_stateMachine->changeState(NavigatorStates::JUMP);
					else if(edge->getType() == NavigationEdgeType::DESCEND)
						_stateMachine->changeState(NavigatorStates::DESCEND);
				}
			}
		}

		void Fall::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const ActionStateID::Enum& state = *(ActionStateID::Enum*)message.getParam();
				if(state == ActionStateID::FALL)
					_stateMachine->changeState(NavigatorStates::WALK);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
			}
		}

		void Jump::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const ActionStateID::Enum& state = *(ActionStateID::Enum*)message.getParam();
				if(state == ActionStateID::CLIMB || state == ActionStateID::JUMP_END)
					_stateMachine->changeState(NavigatorStates::WALK);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_UP));
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
			}
		}

		void Descend::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const ActionStateID::Enum& state = *(ActionStateID::Enum*)message.getParam();
				if(state == ActionStateID::DROP)
					_stateMachine->changeState(NavigatorStates::WALK);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_DOWN));
			}
		}
	}

	StateCollection Navigator::getStates(void) const
	{
		StateCollection states;
		using namespace NavigatorStates;
		states.push_back(new Wait());
		states.push_back(new Walk());
		states.push_back(new Turn());
		states.push_back(new Fall());
		states.push_back(new Jump());
		states.push_back(new Descend());
		return states;
	}

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			const Point& position = *(Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
			const NavigationNode* current = NavigationGraph::get().getNodeByPosition(position);
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