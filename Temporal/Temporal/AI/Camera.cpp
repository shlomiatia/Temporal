#include "Camera.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageParams.h>

namespace Temporal
{
	namespace CameraStates
	{
		const float Search::SEARCH_TIME_FOR_SIDE_IN_MILLIS(5000.0f);

		void Search::enter(void)
		{
			_timer.reset();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::SEARCH)));
		}

		void Search::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(CameraStates::ACQUIRE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(float*)message.getParam();
				_timer.update(framePeriodInMillis);
				if(_timer.getElapsedTimeInMillis() >= SEARCH_TIME_FOR_SIDE_IN_MILLIS)
					_stateMachine->changeState(CameraStates::TURN);
			}
		}

		void See::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::SEE)));
			_haveLineOfSight = false;
		}

		void See::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_haveLineOfSight)
					_stateMachine->changeState(CameraStates::SEARCH);
				_haveLineOfSight = false;
			}
		}

		void Turn::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::TURN)));
			_hasTurned = false;
		}

		void Turn::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				if(_hasTurned)
				{
					_stateMachine->changeState(CameraStates::SEARCH);
				}
				else
				{
					_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
					_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::TURN, true)));
					_hasTurned = true;
				}
			}
		}

		const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

		void Acquire::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::SEARCH)));
			_timer.reset();
			_blinking = false;
			_haveLineOfSight = true;
		}

		void Acquire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				if(_blinking)
					_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::SEARCH)));
				else
					_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(CameraStates::SEE)));
				_blinking = !_blinking;
			}
			else if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_haveLineOfSight = true;
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(float*)message.getParam();
				_timer.update(framePeriodInMillis);

				if(!_haveLineOfSight)
					_stateMachine->changeState(CameraStates::SEARCH);
				else if(_timer.getElapsedTimeInMillis() >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(CameraStates::SEE);
				_haveLineOfSight = false;
			}
		}
	}

	std::vector<ComponentState*> Camera::getStates(void) const
	{
		std::vector<ComponentState*> states;
		using namespace CameraStates;
		states.push_back(new Search());
		states.push_back(new See());
		states.push_back(new Turn());
		states.push_back(new Acquire());
		return states;
	}

}