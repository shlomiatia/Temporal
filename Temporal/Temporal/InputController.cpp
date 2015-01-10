#include "InputController.h"
#include "Keyboard.h"
#include "BaseEnums.h"
#include "MessageUtils.h"
#include "Input.h"
#include "Shapes.h"

namespace Temporal
{
	const Hash InputController::TYPE = Hash("input-controller");

	void InputController::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			Keyboard::get().add(this);
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
			if(Keyboard::get().getKey(Key::SPACE) || Keyboard::get().getKey(Key::W) || (Input::get().getGamepad().getLeftStick().getY() <= 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_UP));
			}
			if(Keyboard::get().getKey(Key::S) || (Input::get().getGamepad().getLeftStick().getY() > 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
			if(Keyboard::get().getKey(Key::Q) || Input::get().getGamepad().getButton(GamepadButton::FRONT_LEFT))
			{
				//raiseMessage(Message(MessageID::ACTION_ACTIVATE));
				//getEntity().getManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));

				/*const OBBAABBWrapper& bounds = *static_cast<const OBBAABBWrapper*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
				getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<OBB*>(&bounds.getOBB())));*/
			}
		}
	}
}