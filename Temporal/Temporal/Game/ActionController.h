#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\Segment.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	class Sensor;

	// TODO: Broder
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
		HangDescendHelper(void) : _platform(Segment::Empty) {}

		void setPlatformFromSensor(const Sensor& sensor);
		const Segment& getPlatform(void) const { return _platform; }
	private:
		Segment _platform;

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

	class ActionState : public ComponentState
	{
	public:
		~ActionState(void) {};

		virtual const char* getName(void) const = 0;

	protected:
		bool isBodyCollisionMessage(Message& message, Direction::Enum positive, Direction::Enum negative = Direction::NONE) const;
		bool isSensorMessage(Message& message, SensorID::Enum sensorID) const;
	};

	class Stand : public ActionState
	{
	public:
		Stand(void) : _isDescending(false) {};

		const char* getName(void) const { return "Stand"; }

		void enter(void);
		void handleMessage(Message& message);

	private:
		bool _isDescending;
	};

	class Fall : public ActionState
	{
	public:
		Fall(void) {};

		const char* getName(void) const { return "Fall"; }

		void enter(void);
		void handleMessage(Message& message);
	};

	class Walk : public ActionState
	{
	public:
		Walk(void) : _stillWalking(false) {};

		const char* getName(void) const { return "Walk"; }

		void enter(void);		
		void handleMessage(Message& message);

	private:
		bool _stillWalking;
	};

	class Turn : public ActionState
	{
	public:
		Turn(void) {};

		const char* getName(void) const { return "Turn"; }

		void enter(void);
		void handleMessage(Message& message);
	};

	class PrepareToJump : public ActionState
	{
	public:
		const char* getName(void) const { return "PrepareToJump"; }

		void enter(void);
		void handleMessage(Message& message);

	private:
		void handleJumpSensor(Message &message);
	};

	class JumpStart : public ActionState
	{
	public:
		JumpStart(void) : _animationEnded(false) {};

		const char* getName(void) const { return "JumpStart"; }

		void enter(void);
		void handleMessage(Message& message);

	private:
		bool _animationEnded;
	};

	class Jump : public ActionState
	{
	public:
		Jump(void) {};

		const char* getName(void) const { return "Jump"; }

		void enter(void);
		void handleMessage(Message& message);
	};

	class JumpEnd : public ActionState
	{
	public:
		JumpEnd(void) {};

		const char* getName(void) const { return "JumpEnd"; }

		void enter(void);
		void handleMessage(Message& message);
	};

	class PrepareToHang : public ActionState
	{
	public:
		PrepareToHang(void) : _platform(NULL) {};

		const char* getName(void) const { return "PrepareToHang"; }

		void enter(void);
		void handleMessage(Message& message);

	private:
		const Segment* _platform;

		void update(void);
	};

	class Hanging : public ActionState
	{
	public:
		Hanging(void){};

		const char* getName(void) const { return "Hanging"; }

		void enter(void);
		void handleMessage(Message& message);
	};

	class Hang : public ActionState
	{
	public:
		Hang(void) {};

		const char* getName(void) const { return "Hang"; }

		void enter(void);
		void handleMessage(Message& message);
	};

	class Drop : public ActionState
	{
	public:
		Drop(void) : _platformFound(false) {};

		const char* getName(void) const { return "Drop"; }

		void enter(void);
		void exit(void);
		void handleMessage(Message& message);

	private:
		bool _platformFound;
	};

	class Climb : public ActionState
	{
	public:
		Climb(void) {};

		const char* getName(void) const { return "Climb"; }
	
		void enter(void);
		void exit(void);
		void handleMessage(Message& message);
	};

	class PrepareToDescend : public ActionState
	{
	public:
		PrepareToDescend(void) : _platform(NULL) {};

		const char* getName(void) const { return "PrepareToDescend"; }

		void enter(void);
		void handleMessage(Message& message);

	private:
		const Segment* _platform;

		void update(void);
	};

	class Descend : public ActionState
	{
	public:
		Descend(void) {};

		const char* getName(void) const { return "Descend"; }

		void enter(void);
		void handleMessage(Message& message);
	};
}

#endif