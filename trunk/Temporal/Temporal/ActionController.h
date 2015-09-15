#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include "Vector.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "StateMachineComponent.h"
#include "Sensor.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Helper classes
	 *********************************************************************************************/
	class JumpInfo
	{
	public:
		JumpInfo(float angle, Hash jumpAnimation, Hash endAnimation)
			: _angle(angle), _jumpAnimation(jumpAnimation), _endAnimation(endAnimation) {}

		float getAngle() const { return _angle; }
		Hash getJumpAnimation() const { return _jumpAnimation; }
		Hash getEndAnimation() const { return _endAnimation; }

	private:
		float _angle;
		Hash _jumpAnimation;
		Hash _endAnimation;

		JumpInfo(const JumpInfo&);
		JumpInfo& operator=(const JumpInfo&);
	};

	namespace JumpType
	{
		enum Enum
		{
			UP,
			FORWARD
		};
	}

	class JumpHelper
	{
	public:
		explicit JumpHelper(JumpType::Enum type) : _type(type) {}

		JumpType::Enum getType() const { return _type; }
		const JumpInfo& getInfo() const { return _type == JumpType::UP ? JUMP_UP_INFO : JUMP_FORWARD_INFO; }
	private:
		static const JumpInfo JUMP_UP_INFO;
		static const JumpInfo JUMP_FORWARD_INFO;

		JumpType::Enum _type;

		JumpHelper(const JumpHelper&);
		JumpHelper& operator=(const JumpHelper&);

		friend class SerializationAccess;
	};

	class LedgeDetector : public ContactListener
	{
	public:
		explicit LedgeDetector(Hash sensorId, Component& owner)
			: ContactListener(sensorId, owner), _up(0), _upFailed(false), _down(0), _downFailed(false), _front(0), _frontFailed(false), _height(0.0f), _max(0.0f), _body(0), _side(Side::LEFT) {}

	protected:
		void start();
		void handle(const Contact& contact);
		void end();

	private:
		Fixture* _up;
		bool _upFailed;
		Fixture* _down;
		bool _downFailed;
		Fixture* _front;
		bool _frontFailed;
		float _height;
		float _max;
		OBBAABBWrapper _body;
		Side::Enum _side;

		void handleUp(const Contact& contact);
		void handleDown(const Contact& contact);
		void handleFront(const Contact& contact);

		void handleFrontCheckY(float y);
	};

	class HandleMessageHelper
	{
	public:
		HandleMessageHelper(StateMachineComponent& controller) : _controller(controller), _isDescending(false), _isActivating(false), _isTakingDown(false) {}

		bool handleStandWalkMessage(Message& message);
		bool handleFallJumpMessage(Message& message);
		bool handleWalkDragMessage(Message& message);
		void handleMessage(Message& message);

	private:
		bool _isDescending;
		bool _isActivating;
		bool _isTakingDown;

		StateMachineComponent& _controller;
	};

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(float maxWalkForcePerSecond = 125.0f);

		Hash getType() const { return TYPE; }

		HandleMessageHelper& getHandleMessageHelper() { return _handleMessageHelper; }

		void handleMessage(Message& message);
		Component* clone() const { return new ActionController(MAX_WALK_FORCE_PER_SECOND); }

		float MAX_WALK_FORCE_PER_SECOND;
		static float WALK_ACC_PER_SECOND;
		static float JUMP_FORCE_PER_SECOND;
		static float FALL_ALLOW_JUMP_TIME;
		static float JUMP_STOP_MODIFIER;
		static float MAX_WALK_JUMP_MODIFIER;
		static const Hash TYPE;

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
		HandleMessageHelper _handleMessageHelper;
		Vector _hangDescendOriginalTranslation;
		Vector _hangDescendGroundDelta;
		Vector _hangDescendMovement;
		Hash _draggableId;

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
			void handleMessage(Message& message) {}
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

		class Drag : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};
	}
}

#endif