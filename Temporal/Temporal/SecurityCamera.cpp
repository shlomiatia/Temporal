#include "SecurityCamera.h"
#include "MessageUtils.h"

namespace Temporal
{
	const Hash SecurityCamera::TYPE = Hash("security-camera");

	namespace SecurityCameraStates
	{
		static Hash SEARCH_ANIMATION = Hash("CAM_ANM_SEARCH");
		static Hash SEE_ANIMATION = Hash("CAM_ANM_SEE");
		static Hash TURN_ANIMATION = Hash("CAM_ANM_TURN");
		static Hash ACQUIRE_ANIMATION = Hash("CAM_ANM_ACQUIRE");

		static const Hash SEARCH_STATE = Hash("CAM_STT_SEARCH");
		static const Hash SEE_STATE = Hash("CAM_STT_SEE");
		static const Hash TURN_STATE = Hash("CAM_STT_TURN");
		static const Hash ACQUIRE_STATE = Hash("CAM_STT_ACQUIRE");

		const float Search::SEARCH_TIME(5.0f);

		void Search::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(SEARCH_ANIMATION)));
		}

		void Search::handleMessage(Message& message)
		{	
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->changeState(ACQUIRE_STATE);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(_stateMachine->getTimer().getElapsedTime() >= SEARCH_TIME)
					_stateMachine->changeState(TURN_STATE);
			}
		}

		void See::enter(void* param)
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(SEE_ANIMATION)));
		}

		void See::handleMessage(Message& message)
		{	
			// TempFlag 1 - Have line of sight
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				if(!_stateMachine->getFrameFlag1())
					_stateMachine->changeState(SEARCH_STATE);
			}
		}

		void Turn::enter(void* param)
		{
			_hasTurned = false;
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(TURN_ANIMATION)));
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
					_stateMachine->raiseMessage(Message(MessageID::FLIP_ORIENTATION));
					_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(TURN_ANIMATION, true)));
					_hasTurned = true;
				}
			}
		}

		const float Acquire::ACQUIRE_TIME(1.0f);

		void Acquire::enter(void* param)
		{
			// TempFlag 1 - have LOS
			_stateMachine->setFrameFlag1(true);
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &AnimationParams(ACQUIRE_ANIMATION)));
		}

		void Acquire::update() const
		{
			float elapsedTime = _stateMachine->getTimer().getElapsedTime();
			if(!_stateMachine->getFrameFlag1())
				_stateMachine->changeState(SEARCH_STATE);
			else if(elapsedTime >= ACQUIRE_TIME)
				_stateMachine->changeState(SEE_STATE);
		}

		void Acquire::handleMessage(Message& message)
		{
			if(message.getID() == MessageID::LINE_OF_SIGHT)
			{
				_stateMachine->setFrameFlag1(true);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				update();
			}
		}
	}

	using namespace SecurityCameraStates;

	StateCollection SecurityCamera::getStates() const
	{
		StateCollection states;
		states[SEARCH_STATE] = new Search();
		states[SEE_STATE] = new See();
		states[TURN_STATE] = new Turn();
		states[ACQUIRE_STATE] = new Acquire();
		return states;
	}

	Hash SecurityCamera::getInitialState() const
	{
		return SEARCH_STATE;
	}
}