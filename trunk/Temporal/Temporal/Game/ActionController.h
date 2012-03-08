#pragma once

#include <Temporal/Game/StateMachineComponent.h>
#include <Temporal/Physics/Body.h>

namespace Temporal
{
	// TODO: Where to put movement constants
	static const float WALK_FORCE_PER_SECOND = 120.0f;
	static const float JUMP_FORCE_PER_SECOND = 1000.0f;

	namespace AnimationID
	{
		enum Enum
		{
			STAND,
			TURN,
			DROP,
			FALL_START,
			FALL,
			JUMP_UP_START,
			JUMP_UP,
			HANG,
			CLIMB,
			JUMP_FORWARD_START,
			JUMP_FORWARD,
			JUMP_FORWARD_END,
			WALK,
			HANG_FORWARD,
			HANG_BACKWARD,
		};
	}

	namespace ActionStateID
	{
		enum Enum
		{
			STAND,
			FALL,
			WALK,
			TURN,
			PREPARE_TO_JUMP,
			JUMP_START_45,
			JUMP_START_60,
			JUMP_START_75,
			JUMP_START_90,
			JUMP_START_105,
			JUMP_UP,
			JUMP_FORWARD,
			JUMP_FORWARD_END,
			PREPARE_TO_HANG,
			HANGING,
			HANG,
			DROP,
			CLIMB,
			PREPARE_TO_DESCEND,
			DESCEND
		};
	}

	// TODO: Divide according to capabilities. Scripted States
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(void) : StateMachineComponent(getStates()) {}
		virtual ComponentType::Enum getType(void) const { return ComponentType::ACTION_CONTROLLER; }

	protected:
		virtual int getInitialState(void) const { return ActionStateID::STAND; }

	private:
		std::vector<ComponentState*> getStates() const;
	};

	class ActionState : public ComponentState
	{
	public:
		virtual ~ActionState(void) {};

		virtual const char* getName(void) const = 0;

	protected:
		bool isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative = Direction::NONE) const;
		bool isSensorMessage(Message& message, SensorID::Enum sensorID) const;
	};

	class Stand : public ActionState
	{
	public:
		Stand(void) : _isDescending(false) {};

		virtual const char* getName(void) const { return "Stand"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public ActionState
	{
	public:
		Fall(void) {};

		virtual const char* getName(void) const { return "Fall"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Walk : public ActionState
	{
	public:
		Walk(void) : _stillWalking(false) {};

		virtual const char* getName(void) const { return "Walk"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _stillWalking;
	};

	class Turn : public ActionState
	{
	public:
		Turn(void) {};

		virtual const char* getName(void) const { return "Turn"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToJump : public ActionState
	{
	public:
		PrepareToJump(void) : _jumpStartState(ActionStateID::JUMP_START_45) {};

		virtual const char* getName(void) const { return "PrepareToJump"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		static const int JUMP_ANGLES_SIZE;
		static const float JUMP_ANGLES[];
		static const ActionStateID::Enum JUMP_ANGLES_START_STATES[];

		ActionStateID::Enum _jumpStartState;

		void handleJumpSensor(Message &message);
	};

	class JumpStart : public ActionState
	{
	public:
		JumpStart(float angle, AnimationID::Enum animation, ActionStateID::Enum jumpState) : 
		  _angle(angle), _animation(animation), _jumpState(jumpState), _platformFound(false), _animationEnded(false) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		float _angle;
		AnimationID::Enum _animation;
		ActionStateID::Enum _jumpState;
		bool _platformFound;
		bool _animationEnded;
	};

	class JumpUp : public ActionState
	{
	public:
		JumpUp(void) {};

		virtual const char* getName(void) const { return "JumpUp"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForward : public ActionState
	{
	public:
		JumpForward(void) {};

		virtual const char* getName(void) const { return "JumpForward"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpForwardEnd : public ActionState
	{
	public:
		JumpForwardEnd(void) {};

		virtual const char* getName(void) const { return "JumpForwardEnd"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToHang : public ActionState
	{
	public:
		PrepareToHang(void) : _platform(NULL) {};

		virtual const char* getName(void) const { return "PrepareToHang"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		const Body* _platform;

		void update(void);
	};

	class Hanging : public ActionState
	{
	public:
		Hanging(void){};

		virtual const char* getName(void) const { return "Hanging"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Hang : public ActionState
	{
	public:
		Hang(void) {};

		virtual const char* getName(void) const { return "Hang"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class Drop : public ActionState
	{
	public:
		Drop(void) : _platformFound(false) {};

		virtual const char* getName(void) const { return "Drop"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _platformFound;
	};

	class Climb : public ActionState
	{
	public:
		Climb(void) {};

		virtual const char* getName(void) const { return "Climb"; }
	protected:
		virtual void handleMessage(Message& message);
	};

	class PrepareToDescend : public ActionState
	{
	public:
		PrepareToDescend(void) : _platform(NULL) {};

		virtual const char* getName(void) const { return "PrepareToDescend"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		const Body* _platform;

		void update(void);
	};

	class Descend : public ActionState
	{
	public:
		Descend(void) {};

		virtual const char* getName(void) const { return "Descend"; }
	protected:
		virtual void handleMessage(Message& message);
	};
}
