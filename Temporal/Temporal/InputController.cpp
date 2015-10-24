#include "InputController.h"
#include "BaseEnums.h"
#include "MessageUtils.h"
#include "Input.h"
#include "Shapes.h"
#include "Keyboard.h"

namespace Temporal
{
	const float JUMP_LEEWAY = 0.1f;
	const Hash PLAYER_PERIOD = Hash("player-period");
	const Hash TEMPORAL_ECHO_MANAGER = Hash("temporal-echo-manager");
	const Hash InputController::TYPE = Hash("input-controller");

	void InputController::startJump()
	{
		_isJump = true;
		_timer.reset();
	}

	void InputController::temporalAction()
	{
		if (getEntity().get(PLAYER_PERIOD) || getEntity().get(TEMPORAL_ECHO_MANAGER)) {
			raiseMessage(Message(MessageID::ACTION_TEMPORAL_TRAVEL));
		}
	}

	void InputController::mouseDown(Message& message)
	{
		const MouseParams& params = getMouseParams(message.getParam());
		if (params.getButton() == MouseButton::LEFT)
		{
			raiseMessage(Message(MessageID::ACTION_TAKEDOWN));
		}
	}

	void InputController::keyDown(Message& message)
	{
		Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
		if (key == Key::SPACE)
		{
			startJump();
		}
		else if (key == Key::E)
		{
			raiseMessage(Message(MessageID::ACTION_ACTIVATE));
		}
		else if (key == Key::Q)
		{
			temporalAction();
		}
	}

	void InputController::gamepadButtonDown(Message& message)
	{
		GamepadButton::Enum button = *static_cast<GamepadButton::Enum*>(message.getParam());
		if (button == GamepadButton::ACTION_DOWN)
		{
			startJump();
		}
		else if (button == GamepadButton::ACTION_LEFT)
		{
			raiseMessage(Message(MessageID::ACTION_TAKEDOWN));
		}
		else if (button == GamepadButton::ACTION_RIGHT)
		{
			raiseMessage(Message(MessageID::ACTION_ACTIVATE));
		}
		else if (button == GamepadButton::SHOULDER_LEFT)
		{
			temporalAction();
		}
	}

	void InputController::update(float time)
	{
		if (Keyboard::get().getKey(Key::D) || Input::get().getGamepad().getLeftStick().getX() > 0.0f)
		{
			sendDirectionAction(*this, Side::RIGHT);
		}
		if (Keyboard::get().getKey(Key::A) || Input::get().getGamepad().getLeftStick().getX() < 0.0f)
		{
			sendDirectionAction(*this, Side::LEFT);
		}
		if (_isJump)
		{

			_timer.update(time);
			if (_timer.getElapsedTime() < JUMP_LEEWAY)
				raiseMessage(Message(MessageID::ACTION_UP_START));
			else
				_isJump = false;
		}
		if (Keyboard::get().getKey(Key::SPACE) || Input::get().getGamepad().getButton(GamepadButton::ACTION_DOWN))
		{
			raiseMessage(Message(MessageID::ACTION_UP_CONTINUE));
		}
		if (Keyboard::get().getKey(Key::S) || Input::get().getGamepad().getLeftStick().getY() > 0.0f)
		{
			raiseMessage(Message(MessageID::ACTION_DOWN));
		}
		// TEMP:
		if (Keyboard::get().getKey(Key::TAB))
		{
			Hash id = Hash("ENT_PLAYER");
			getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER0"), Message(MessageID::SET_NAVIGATION_DESTINATION, &id));
		}
	}

	void InputController::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::MOUSE_DOWN)
		{
			mouseDown(message);
		}
		else if (message.getID() == MessageID::KEY_DOWN)
		{
			keyDown(message);
		}
		else if (message.getID() == MessageID::GAMEPAD_BUTTON_DOWN)
		{
			gamepadButtonDown(message);
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float time = getFloatParam(message.getParam());
			update(time);
		}
	}
}