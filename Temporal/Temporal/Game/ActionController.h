#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <Temporal\Base\Hash.h>
#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\Timer.h>
#include <Temporal\Game\StateMachineComponent.h>

namespace Temporal
{
	class Point;
	class SensorCollisionParams;

	// BRODER
	static const float WALK_FORCE_PER_SECOND = 150.0f;
	static const float JUMP_FORCE_PER_SECOND = 900.0f;

	class JumpInfo
	{
	public:
		JumpInfo(float angle, const Hash& startAnimation, const Hash& jumpAnimation, const Hash& endAnimation)
			: _angle(angle), _startAnimation(startAnimation), _jumpAnimation(jumpAnimation), _endAnimation(endAnimation) {}

		float getAngle(void) const { return _angle; }
		Hash getStartAnimation(void) const { return _startAnimation; }
		Hash getJumpAnimation(void) const { return _jumpAnimation; }
		Hash getEndAnimation(void) const { return _endAnimation; }

		bool operator==(const JumpInfo& other) const { return getAngle() == other.getAngle(); }
		bool operator!=(const JumpInfo& other) const { return !(*this == other); }

	private:
		float _angle;
		Hash _startAnimation;
		Hash _jumpAnimation;
		Hash _endAnimation;

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
		HangDescendHelper(void) : _point(NULL) {}

		void setPoint(const SensorCollisionParams& params);
		const Point& getPoint(void) const { return *_point; }
	private:
		const Point* _point;

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
		Hash getInitialState(void) const { return Hash("STAT_ACT_STAND"); }

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
		Fall(void) : _wantToHang(false) {};

		void enter(void);
		void handleMessage(Message& message);
		
	private:
		bool _wantToHang;
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
		Jump(void) : _wantToHang(false) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
		bool _wantToHang;
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
		PrepareToHang(void) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
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
		PrepareToDescend(void) {};

		void enter(void);
		void handleMessage(Message& message);

	private:
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