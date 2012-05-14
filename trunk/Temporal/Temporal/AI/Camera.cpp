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

		void Search::enter(void) const
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEARCH_ANIMATION)));
		}

		void Search::handleMessage(Message& message) const
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float framePeriodInMillis = *(float*)message.getParam();
				if(_stateMachine->getTimer().getElapsedTimeInMillis() >= SEARCH_TIME_FOR_SIDE_IN_MILLIS)
					_stateMachine->changeState(TURN_STATE);
			}
		}

		void See::enter(void) const
		{
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEE_ANIMATION)));
		}

		void See::handleMessage(Message& message) const
		{	
			// Flag 1 - Have line of sight
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getFlag1())
					_stateMachine->changeState(SEARCH_STATE);
			}
		}

		void Turn::enter(void) const
		{
			_hasTurned = false;
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(TURN_ANIMATION)));
		}

		void Turn::handleMessage(Message& message) const
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
		const float Acquire::BLINK_TIME_IN_MILLIS(200.0f);

		void Acquire::enter(void) const
		{
			// Flag 1 - have LOS
			_stateMachine->setFlag1(true);
			_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEARCH_ANIMATION)));
		}

		void Acquire::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				float elapsedTimeInMillis = _stateMachine->getTimer().getElapsedTimeInMillis();
				if(!_stateMachine->getFlag1())
					_stateMachine->changeState(SEARCH_STATE);
				else if(elapsedTimeInMillis >= ACQUIRE_TIME_IN_MILLIS)
					_stateMachine->changeState(SEE_STATE);
				else
				{
					int blinkIndex = (int)(elapsedTimeInMillis / BLINK_TIME_IN_MILLIS);
					if(blinkIndex % 2 == 1)
						_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEARCH_ANIMATION)));
					else
						_stateMachine->sendMessageToOwner(Message(MessageID::RESET_ANIMATION, &ResetAnimationParams(SEE_ANIMATION)));
				}
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