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
			: ContactListener(sensorId, owner), _upFound(false), _upFailed(false), _downFound(false), _downFailed(false), 
										 _frontFound(false), _frontFailed(false), _height(0.0f), _max(0.0f), _body(0), _side(Side::LEFT) {}

	protected:
		void start();
		void handle(const Contact& contact);
		void end();

	private:
		bool _upFound;
		bool _upFailed;
		bool _downFound;
		bool _downFailed;
		bool _frontFound;
		bool _frontFailed;
		float _height;
		float _max;
		const OBBAABBWrapper* _body;
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
		ActionController();

		Hash getType() const { return TYPE; }

		HandleMessageHelper& getHandleMessageHelper() { return _handleMessageHelper; }

		void handleMessage(Message& message);
		Component* clone() const { return new ActionController(); }

		float MAX_WALK_FORCE_PER_SECOND;
		static float WALK_ACC_PER_SECOND;
		static float JUMP_FORCE_PER_SECOND;
		static float FALL_ALLOW_JUMP_TIME;
		static float JUMP_STOP_MODIFIER;
		static float MAX_WALK_JUMP_MODIFIER;
		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:
		LedgeDetector _ledgeDetector;
		HandleMessageHelper _handleMessageHelper;

		StateCollection getStates() const;
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

		class Slide : public ComponentState
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
			void enter(void* param);
			void exit(void* param);
			void handleMessage(Message& message);
		};

		class Descend : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
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
	}
}

#endif