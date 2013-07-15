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
		if(message.getID() == MessageID::UPDATE)
		{
			if(Keyboard::get().getKey(Key::RIGHT) || Input::get().getGamepad().getLeftStick().getX() > 0.0f)
			{
				sendDirectionAction(*this, Side::RIGHT);
			}
			if(Keyboard::get().getKey(Key::LEFT)  || Input::get().getGamepad().getLeftStick().getX() < 0.0f)
			{
				sendDirectionAction(*this, Side::LEFT);
			}
			if(Keyboard::get().getKey(Key::UP) || (Input::get().getGamepad().getLeftStick().getY() <= 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_UP));
			}
			if(Keyboard::get().getKey(Key::DOWN) || (Input::get().getGamepad().getLeftStick().getY() > 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
			if(Keyboard::get().getKey(Key::Q)  || Input::get().getGamepad().getButton(GamepadButton::FRONT_LEFT))
			{
				raiseMessage(Message(MessageID::ACTION_ACTIVATE));
			}
		}
	}
}