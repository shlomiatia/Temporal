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
			if(Keyboard::get().isKeyPressed(Key::D) || Input::get().getGamepad().getLeftStick().getX() > 0.0f)
			{
				sendDirectionAction(*this, Side::RIGHT);
			}
			if(Keyboard::get().isKeyPressed(Key::A) == KeyboardEvent::DOWN || Input::get().getGamepad().getLeftStick().getX() < 0.0f)
			{
				sendDirectionAction(*this, Side::LEFT);
			}
			if(Keyboard::get().isKeyPressed(Key::SPACE) || Keyboard::get().isKeyPressed(Key::W) || (Input::get().getGamepad().getLeftStick().getY() <= 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_UP));
			}
			if(Keyboard::get().isKeyPressed(Key::S) || (Input::get().getGamepad().getLeftStick().getY() > 0.0f && Input::get().getGamepad().getButton(GamepadButton::FRONT_DOWN)))
			{
				raiseMessage(Message(MessageID::ACTION_DOWN));
			}
			if(Keyboard::get().isKeyDown(Key::Q)  || Input::get().getGamepad().getButton(GamepadButton::FRONT_LEFT))
			{
				raiseMessage(Message(MessageID::ACTION_ACTIVATE));
			}
		}
	}
}