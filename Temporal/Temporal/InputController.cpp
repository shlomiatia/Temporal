#include "InputController.h"
#include "Keyboard.h"
#include "BaseEnums.h"
#include "MessageUtils.h"
#include "Input.h"

namespace Temporal
{
	const Hash InputController::TYPE = Hash("input-controller");

	void InputController::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::KEY_DOWN)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if(key == Key::D || Input::get().getGamepad().getLeftStick().getX() > 0.0f)
			{
				sendDirectionAction(*this, Side::RIGHT);
			}
			if(key == Key::A || Input::get().getGamepad().getLeftStick().getX() < 0.0f)
			{
				sendDirectionAction(*this, Side::LEFT);
			}
			if(key == Key::SPACE || key == Key::W || (Input::get().getGamepad().getLeftStick().getY() <= 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_UP));
			}
			if(key == Key::S || (Input::get().getGamepad().getLeftStick().getY() > 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
			if(key == Key::Q || Input::get().getGamepad().getButton(GamepadButton::FRONT_LEFT))
			{
				raiseMessage(Message(MessageID::ACTION_ACTIVATE));
			}
		}
	}
}