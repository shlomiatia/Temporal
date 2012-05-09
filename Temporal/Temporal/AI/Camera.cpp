#include "Camera.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Game\MessageParams.h>

namespace Temporal
{
	namespace CameraStates
	{
		static const Hash SEARCH_ANIMATION = Hash("ANIM_CAM_SEARCH");
		static const Hash SEE_ANIMATION = Hash("ANIM_CAM_SEE");
		static const Hash TURN_ANIMATION = Hash("ANIM_CAM_TURN");

		static const Hash SEARCH_STATE = Hash("STAT_CAM_SEARCH");
		static const Hash SEE_STATE = Hash("STAT_CAM_SEE");
		static const Hash TURN_STATE = Hash("STAT_CAM_TURN");
		static const Hash ACQUIRE_STATE = Hash("STAT_CAM_ACQUIRE");

		const float Search::SEARCH_TIME_FOR_SIDE_IN_MILLIS(5000.0f);

		void Search::enter(void)
		{
			_timer.reset();
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEARCH_ANIMATION)));
		}

		void Search::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(float*)message.getParam();
				_timer.update(framePeriodInMillis);
				if(_timer.getElapsedTimeInMillis() >= SEARCH_TIME_FOR_SIDE_IN_MILLIS)
					_stateMachine->changeState(TURN_STATE);
			}
		}

		void See::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEE_ANIMATION)));
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
					_stateMachine->changeState(SEARCH_STATE);
				_haveLineOfSight = false;
			}
		}

		void Turn::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(TURN_ANIMATION)));
			_hasTurned = false;
		}

		void Turn::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				if(_hasTurned)
				{
					_stateMachine->changeState(SEARCH_STATE);
				}
				else
				{
					_stateMachine->sendMessageToOwner(Message(MessageID::FLIP_ORIENTATION));
					_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(TURN_ANIMATION, true)));
					_hasTurned = true;
				}
			}
		}

		const float Acquire::ACQUIRE_TIME_IN_MILLIS(1000.0f);

		void Acquire::enter(void)
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEARCH_ANIMATION)));
			_timer.reset();
			_blinking = false;
			_haveLineOfSight = true;
		}

		void Acquire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				if(_blinking)
					_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEARCH_ANIMATION)));
				else
					_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEE_ANIMATION)));
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
					_stateMachine->changeState(SEARCH_STATE);
				else if(_timer.getElapsedTimeInMillis() >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(SEE_STATE);
				_haveLineOfSight = false;
			}
		}
	}

	using namespace CameraStates;

	StateCollection Camera::getStates(void) const
	{
		StateCollection states;
		states[SEARCH_STATE] = new Search();
		states[SEE_STATE] = new See();
		states[TURN_STATE] = new Turn();
		states[ACQUIRE_STATE] = new Acquire();
		return states;
	}

	Hash Camera::getInitialState(void) const
	{
		return SEARCH_STATE;
	}
}