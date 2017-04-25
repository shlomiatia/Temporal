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
	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(float maxWalkForcePerSecond = 125.0f, Hash initialStateId = Hash::INVALID);

		Hash getType() const { return TYPE; }

		CommonMessagesHandler& getHandleMessageHelper() { return _handleMessageHelper; }

		void handleMessage(Message& message);
		Component* clone() const { return new ActionController(MAX_WALK_FORCE_PER_SECOND, getCurrentStateID()); }

		float MAX_WALK_FORCE_PER_SECOND;
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
		Hash getDraggableId() const { return _draggableId; }
		void setDraggableId(Hash value) { _draggableId = value; }

	protected:
		Hash getInitialState() const;

	private:
		LedgeDetector _ledgeDetector;
		CommonMessagesHandler _handleMessageHelper;
		Vector _hangDescendOriginalTranslation;
		Vector _hangDescendGroundDelta;
		Vector _hangDescendMovement;
		Hash _draggableId;
		bool _isInvestigated;

		HashStateMap getStates() const;

		friend class SerializationAccess;
	};

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/
	namespace ActionControllerStates
	{
		class Stand : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Fall : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Walk : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Turn : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Jump : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class JumpEnd : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Hang : public ComponentState
		{
		public:
			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);
		};

		class Climb : public ComponentState
		{
		public:
			Climb() {}

			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);

		private:
		};

		class Descend : public ComponentState
		{
		public:
			Descend() {}

			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);
		private:
			
		};

		class Takedown : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Die : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Dead : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message) {};
		};

		class Aim : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Fire : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class DragStand : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class DragForward : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class DragBackwards : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Investigate : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif