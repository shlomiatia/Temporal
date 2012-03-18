#include "Navigator.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Graphics\Graphics.h>
#include <math.h>

namespace Temporal
{
	namespace NavigatorStates
	{
		void Wait::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::SET_NAVIGATION_DESTINATION)
			{
				const Vector* goalPosition = (const Vector* const)message.getParam();
				Navigator& navigator = *(Navigator*)_stateMachine;

				// TODO: Whor'e you chasing ENTITIES
				const Vector& startPosition = *(const Vector*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_POSITION));
				const NavigationNode* start = NavigationGraph::get().getNodeByPosition(startPosition);
				const NavigationNode* goal = NavigationGraph::get().getNodeByPosition(*goalPosition);
				if(start != NULL && goal != NULL)
				{
					std::vector<const NavigationEdge* const>* path = Pathfinder::get().findPath(start, goal);
					navigator.setDestination(goalPosition);
					navigator.setPath(path);
					_stateMachine->changeState(NavigatorStates::WALK);
				}
			}
		}

		const float Walk::DISTANCE_FROM_TARGET_TOLERANCE(10.0f);

		void Walk::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				const Vector& position = *(const Vector*)_stateMachine->sendMessageToOwner(Message(MessageID::GET_POSITION));
				float sourceX = position.getX();
				const Navigator& navigator = *((const Navigator*)_stateMachine);
				std::vector<const NavigationEdge* const>* path = navigator.getPath();
				float targetX;
				bool reachedTargetPlatform;
				if(path == NULL)
				{
					const Vector& destination = navigator.getDestination();
					targetX = destination.getX();
					reachedTargetPlatform = true;
				}
				else
				{
					const NavigationEdge* const edge = (*path)[0];
					targetX = edge->getX();
					reachedTargetPlatform = false;
				}
				
				float distance = targetX - sourceX;
				if(abs(distance) <= DISTANCE_FROM_TARGET_TOLERANCE)
				{
					if(reachedTargetPlatform)
						_stateMachine->changeState(NavigatorStates::WAIT);
					else
						_stateMachine->changeState(NavigatorStates::TURN);
				}
				else
				{
					// TODO: Narrow this
					Orientation::Enum orientation = *(const Orientation::Enum* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
					if((distance < 0 && orientation == Orientation::LEFT) || (distance > 0 && orientation == Orientation::RIGHT))
						_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_FORWARD));
					else
						_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_BACKWARD));
				}
			}
		}

		void Turn::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				Navigator* navigator = (Navigator*)_stateMachine;
				std::vector<const NavigationEdge* const>* path = navigator->getPath();
				const NavigationEdge* const edge = (*path)[0];
				Orientation::Enum currentOrientation = *(const Orientation::Enum* const)_stateMachine->sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
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
				}
			}
		}

		void Fall::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::STATE_EXITED)
			{
				const ActionStateID::Enum& state = *(const ActionStateID::Enum* const)message.getParam();
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
				const ActionStateID::Enum& state = *(const ActionStateID::Enum* const)message.getParam();
				if(state == ActionStateID::CLIMB || state == ActionStateID::JUMP_END)
					_stateMachine->changeState(NavigatorStates::WALK);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				_stateMachine->sendMessageToOwner(Message(MessageID::ACTION_UP));
			}
		}
	}

	std::vector<ComponentState*> Navigator::getStates(void) const
	{
		std::vector<ComponentState*> states;
		using namespace NavigatorStates;
		states.push_back(new Wait());
		states.push_back(new Walk());
		states.push_back(new Turn());
		states.push_back(new Fall());
		states.push_back(new Jump());
		return states;
	}

	void Navigator::handleMessage(Message& message)
	{
		StateMachineComponent::handleMessage(message);
		if(message.getID() == MessageID::DEBUG_DRAW)
		{
			const Vector& position = *(const Vector* const)sendMessageToOwner(Message(MessageID::GET_POSITION));
			const NavigationNode* current = NavigationGraph::get().getNodeByPosition(position);

			if(current != NULL && getPath() != NULL)
			{	
				for(unsigned int i = 0; i < getPath()->size(); ++i)
				{
					const NavigationEdge& edge = *((*getPath())[i]);
					const NavigationNode& next = edge.getTarget();
					Graphics::get().drawLine(current->getArea().getCenter(), next.getArea().getCenter(), Color::Cyan);
					current = &next;
				}
			}
		}
	}
}