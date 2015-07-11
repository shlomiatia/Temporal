#include "InputController.h"
#include "Keyboard.h"
#include "BaseEnums.h"
#include "MessageUtils.h"
#include "Input.h"
#include "Shapes.h"

namespace Temporal
{
	const float JUMP_LEEWAY = 0.1f;
	const Hash InputController::TYPE = Hash("input-controller");

	void InputController::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
		}
		else if ((message.getID() == MessageID::KEY_DOWN && *static_cast<Key::Enum*>(message.getParam()) == Key::SPACE) ||
				 (message.getID() == MessageID::GAMEPAD_BUTTON_DOWN && *static_cast<Key::Enum*>(message.getParam()) == GamepadButton::ACTION_DOWN))
		{
			_isJump = true;
			_timer.reset();
 			
		}
		else if((message.getID() == MessageID::GAMEPAD_BUTTON_DOWN && *static_cast<Key::Enum*>(message.getParam()) == GamepadButton::ACTION_LEFT) || 
				 message.getID() == MessageID::MOUSE_DOWN ) 
		{
			raiseMessage(Message(MessageID::ACTION_TAKEDOWN));
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			
			if(Keyboard::get().getKey(Key::D) || Input::get().getGamepad().getLeftStick().getX() > 0.0f)
			{
				sendDirectionAction(*this, Side::RIGHT);
			}
			if(Keyboard::get().getKey(Key::A) || Input::get().getGamepad().getLeftStick().getX() < 0.0f)
			{
				sendDirectionAction(*this, Side::LEFT);
			}
			if(_isJump)
			{
				float time = getFloatParam(message.getParam());
				_timer.update(time);
				if(_timer.getElapsedTime() < JUMP_LEEWAY)
					raiseMessage(Message(MessageID::ACTION_UP_START));
				else
					_isJump = false;
			}
			if(Keyboard::get().getKey(Key::SPACE) || Input::get().getGamepad().getButton(GamepadButton::ACTION_DOWN))
			{
				raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
			}
			if(Keyboard::get().getKey(Key::S) || (Input::get().getGamepad().getLeftStick().getY() > 0.0f))
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
			if(Keyboard::get().getKey(Key::E) || Input::get().getGamepad().getButton(GamepadButton::ACTION_RIGHT))
			{
				raiseMessage(Message(MessageID::ACTION_ACTIVATE));
			}
			if (Keyboard::get().getKey(Key::LEFT_SHIFT) || Input::get().getGamepad().getButton(GamepadButton::ACTION_UP))
			{
				getEntity().getManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
			}
			if (Keyboard::get().getKey(Key::TAB))
			{
				OBB& bounds = *static_cast<OBB*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
				getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, &bounds));
			}
		}
	}
}