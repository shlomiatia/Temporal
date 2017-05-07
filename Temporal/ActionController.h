#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include "Vector.h"
#include "Hash.h"
#include "StateMachineComponent.h"
#include "Sensor.h"
#include "LedgeDetector.h"
#include "CommonMessagesHandler.h"

namespace Temporal
{
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(float maxWalkForcePerSecond = 125.0f, Hash initialStateId = Hash::INVALID);

		Hash getType() const { return TYPE; }

		CommonMessagesHandler& getHandleMessageHelper() { return _handleMessageHelper; }

		void handleMessage(Message& message);
		Component* clone() const { return new ActionController(_maxWalkForcePerSecond, getCurrentStateID()); }

		float _maxWalkForcePerSecond;
		static float WALK_ACC_PER_SECOND;
		static float JUMP_FORCE_PER_SECOND;
		static float FALL_ALLOW_JUMP_TIME;
		static float JUMP_STOP_MODIFIER;
		static float MAX_WALK_JUMP_MODIFIER;
		static const Hash TYPE;

		static ActionController& getActionController(StateMachineComponent* stateMachine);

		const Vector& getHangDescendOriginalTranslation() const { return _hangDescendOriginalTranslation; }
		void setHangDescendOriginalTranslation(const Vector& value) { _hangDescendOriginalTranslation = value; }
		const Vector& getHangDescendGroundDelta() const { return _hangDescendGroundDelta; }
		void setHangDescendGroundDelta(const Vector& value) { _hangDescendGroundDelta = value; }
		const Vector& getHangDescendMovement() const { return _hangDescendMovement; }
		void setHangDescendMovement(const Vector& value) { _hangDescendMovement = value; }

	protected:
		Hash getInitialState() const;

	private:
		LedgeDetector _ledgeDetector;
		CommonMessagesHandler _handleMessageHelper;
		Vector _hangDescendOriginalTranslation;
		Vector _hangDescendGroundDelta;
		Vector _hangDescendMovement;
		bool _isInvestigated;

		HashStateMap getStates() const;
		void die();

		friend class SerializationAccess;
	};
}

#endif