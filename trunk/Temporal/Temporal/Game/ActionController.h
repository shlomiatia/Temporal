#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	class Shape;
	class Sensor;

	// BRODER
	static const float WALK_FORCE_PER_SECOND = 150.0f;
	static const float JUMP_FORCE_PER_SECOND = 900.0f;

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

	typedef std::vector<const JumpInfo*> JumpInfoCollection;
	typedef JumpInfoCollection::const_iterator JumpInfoIterator;

	class JumpInfoProvider
	{
	public:
		static const JumpInfoProvider& get(void)
		{
			static JumpInfoProvider instance;
			return instance;
		}

		const JumpInfoCollection& getData(void) const { return _data; }
		const JumpInfo* getFarthest(void) const { return _data[0]; }
		const JumpInfo* getHighest(void) const { return _data[3]; }
		
	private:
		JumpInfoCollection _data;

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

	class HangDescendHelper
	{
	public:
		HangDescendHelper(void) : _platform(NULL) {}

		void setPlatformFromSensor(const Sensor& sensor);
		const Shape& getPlatform(void) const { return *_platform; }
	private:
		const Shape* _platform;

		HangDescendHelper(const HangDescendHelper&);
		HangDescendHelper& operator=(const HangDescendHelper&);
	};

	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(void) : StateMachineComponent(getStates()) {}

		ComponentType::Enum getType(void) const { return ComponentType::ACTION_CONTROLLER; }
		JumpHelper& getJumpHelper(void) { return _jumpHelper; }
		HangDescendHelper& getHangDescendHelper(void) { return _hangDescendHelper; }

	protected:
		int getInitialState(void) const { return ActionStateID::STAND; }

	private:
		JumpHelper _jumpHelper;
		HangDescendHelper _hangDescendHelper;

		StateCollection getStates() const;
	};

	class Stand : public ComponentState
	{
	public:
		Stand(void) : _isDescending(false) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public ComponentState
	{
	public:
		Fall(void) {};

		void enter(void);
		void handleMessage(Message& message);
	};

	class Walk : public ComponentState
	{
	public:
		Walk(void) : _stillWalking(false), _noFloor(false) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
		static const float NO_FLOOR_TIME_TO_FALL_IN_MILLIS;

		bool _stillWalking;
		bool _noFloor;
		Timer _noFloorTimer;
	};

	class Turn : public ComponentState
	{
	public:
		Turn(void) {};

		void enter(void);
		void handleMessage(Message& message);
	};

	class PrepareToJump : public ComponentState
	{
	public:
		void enter(void);
		void handleMessage(Message& message);

	private:
		void handleJumpSensor(Message &message);
	};

	class JumpStart : public ComponentState
	{
	public:
		JumpStart(void) : _animationEnded(false) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
		bool _animationEnded;
	};

	class Jump : public ComponentState
	{
	public:
		Jump(void) {};

		void enter(void);
		void handleMessage(Message& message);
	};

	class JumpEnd : public ComponentState
	{
	public:
		JumpEnd(void) {};

		void enter(void);
		void handleMessage(Message& message);
	};

	class PrepareToHang : public ComponentState
	{
	public:
		PrepareToHang(void) : _platform(NULL) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
		const Shape* _platform;

		void update(void);
	};

	class Hanging : public ComponentState
	{
	public:
		Hanging(void){};

		void enter(void);
		void handleMessage(Message& message);
	};

	class Hang : public ComponentState
	{
	public:
		Hang(void) {};

		void enter(void);
		void handleMessage(Message& message);
	};

	class Drop : public ComponentState
	{
	public:
		Drop(void) : _platformFound(false) {};

		void enter(void);
		void exit(void);
		void handleMessage(Message& message);

	private:
		bool _platformFound;
	};

	class Climb : public ComponentState
	{
	public:
		Climb(void) {};

		void enter(void);
		void exit(void);
		void handleMessage(Message& message);
	};

	class PrepareToDescend : public ComponentState
	{
	public:
		PrepareToDescend(void) : _platform(NULL) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
		const Shape* _platform;

		void update(void);
	};

	class Descend : public ComponentState
	{
	public:
		Descend(void) {};

		void enter(void);
		void handleMessage(Message& message);
	};
}

#endif