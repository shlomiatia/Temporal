#pragma once

#include <Temporal\Game\StateMachineComponent.h>
#include <Temporal\Physics\Body.h>

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

			JUMP_START,
			JUMP,
			JUMP_END,

			// TODO: Gradual hang & descend
			PREPARE_TO_HANG,
			HANGING,
			HANG,
			DROP,
			CLIMB,
			PREPARE_TO_DESCEND,
			DESCEND
		};
	}

	class JumpInfo
	{
	public:
		JumpInfo(float angle, AnimationID::Enum startAnimation, AnimationID::Enum jumpAnimation, AnimationID::Enum endAnimation)
			: _angle(angle), _startAnimation(startAnimation), _jumpAnimation(jumpAnimation), _endAnimation(endAnimation) {}

		float getAngle(void) const { return _angle; }
		AnimationID::Enum getStartAnimation(void) const { return _startAnimation; }
		AnimationID::Enum getJumpAnimation(void) const { return _jumpAnimation; }
		AnimationID::Enum getEndAnimation(void) const { return _endAnimation; }

		bool operator==(const JumpInfo& other) const { return getAngle() == other.getAngle(); }
		bool operator!=(const JumpInfo& other) const { return !(*this == other); }

	private:
		float _angle;
		AnimationID::Enum _startAnimation;
		AnimationID::Enum _jumpAnimation;
		AnimationID::Enum _endAnimation;

		JumpInfo(const JumpInfo&);
		JumpInfo& operator=(const JumpInfo&);
	};

	class JumpInfoProvider
	{
	public:
		static const JumpInfoProvider& get(void)
		{
			static JumpInfoProvider instance;
			return instance;
		}

		const std::vector<const JumpInfo* const>& getData(void) const { return _data; }
		const JumpInfo* const getFarthest(void) const { return _data[0]; }
		const JumpInfo* const getHighest(void) const { return _data[3]; }
		
	private:
		std::vector<const JumpInfo*const > _data;

		JumpInfoProvider(void);
		~JumpInfoProvider(void);
		JumpInfoProvider(const JumpInfoProvider&);
		JumpInfoProvider& operator=(const JumpInfoProvider&);
	};

	class JumpHelper
	{
	public:
		JumpHelper(void) : _info(NULL), _ledgeDirected(false) {}

		const JumpInfo& getInfo(void) const { return *_info; }
		void setInfo(const JumpInfo* info) { _info = info; }
		bool isLedgeDirected(void) const { return _ledgeDirected; }
		void setLedgeDirected(bool ledgeDirected) { _ledgeDirected = ledgeDirected; }
	private:
		const JumpInfo* _info;
		bool _ledgeDirected;

		JumpHelper(const JumpHelper&);
		JumpHelper& operator=(const JumpHelper&);
	};

	// TODO: Divide according to capabilities. Scripted States
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(void) : StateMachineComponent(getStates()) {}

		virtual ComponentType::Enum getType(void) const { return ComponentType::ACTION_CONTROLLER; }
		JumpHelper& getJumpHelper(void) { return _jumpHelper; }

	protected:
		virtual int getInitialState(void) const { return ActionStateID::STAND; }

	private:
		std::vector<ComponentState*> getStates() const;
		JumpHelper _jumpHelper;
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
		virtual const char* getName(void) const { return "PrepareToJump"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		void handleJumpSensor(Message &message);
	};

	class JumpStart : public ActionState
	{
	public:
		JumpStart(void) : _animationEnded(false) {};

		virtual const char* getName(void) const { return "JumpStart"; }

	protected:
		virtual void handleMessage(Message& message);

	private:
		bool _animationEnded;
	};

	class Jump : public ActionState
	{
	public:
		Jump(void) {};

		virtual const char* getName(void) const { return "Jump"; }

	protected:
		virtual void handleMessage(Message& message);
	};

	class JumpEnd : public ActionState
	{
	public:
		JumpEnd(void) {};

		virtual const char* getName(void) const { return "JumpEnd"; }

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
