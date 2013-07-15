#include "Door.h"

namespace Temporal
{
	const Hash Door::TYPE = Hash("door");

	static Hash OPEN_ANIMATION = Hash("DOR_ANM_OPEN");
	static Hash CLOSE_ANIMATION = Hash("DOR_ANM_CLOSE");
	static Hash OPENING_ANIMATION = Hash("DOR_ANM_OPENING");
	static Hash CLOSING_ANIMATION = Hash("DOR_ANM_CLOSING");

	static const Hash OPEN_STATE = Hash("DOR_STT_OPEN");
	static const Hash CLOSE_STATE = Hash("DOR_STT_CLOSE");
	static const Hash OPENING_STATE = Hash("DOR_STT_OPENING");
	static const Hash CLOSING_STATE = Hash("DOR_STT_CLOSING");

	StateCollection Door::getStates() const
	{
		StateCollection states;
		states[OPEN_STATE] = new DoorStates::Open();
		states[CLOSE_STATE] = new DoorStates::Close();
		states[OPENING_STATE] = new DoorStates::Opening();
		states[CLOSING_STATE] = new DoorStates::Closing();
		return states;
	}

	Hash Door::getInitialState() const { return CLOSE_STATE; }

	namespace DoorStates
	{
		void Open::enter() const
		{
			bool isEnabled = false;
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &isEnabled));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &OPEN_ANIMATION));
		}

		void Open::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ACTIVATE)
			{
				_stateMachine->changeState(CLOSING_STATE);
			}
		}

		void Close::enter() const
		{
			bool isEnabled = true;
			_stateMachine->raiseMessage(Message(MessageID::SET_BODY_ENABLED, &isEnabled));
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &CLOSE_ANIMATION));
		}

		void Close::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ACTIVATE)
			{
				_stateMachine->changeState(OPENING_STATE);
			}
		}

		void Opening::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &OPENING_ANIMATION));
		}

		void Opening::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(OPEN_STATE);
			}
		}

		void Closing::enter() const
		{
			_stateMachine->raiseMessage(Message(MessageID::RESET_ANIMATION, &CLOSING_ANIMATION));
		}

		void Closing::handleMessage(Message& message) const
		{
			if(message.getID() == MessageID::ANIMATION_ENDED)
			{
				_stateMachine->changeState(CLOSE_STATE);
			}
		}
	}
}