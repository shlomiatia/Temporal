#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include "NumericPair.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "StateMachineComponent.h"

namespace Temporal
{
	class SensorCollisionParams;

	// BRODER
	static const float WALK_FORCE_PER_SECOND = 150.0f;
	static const float JUMP_FORCE_PER_SECOND = 900.0f;

	/**********************************************************************************************
	 * Jump Helpers
	 *********************************************************************************************/
	class JumpInfo
	{
	public:
		JumpInfo(const Hash& startAnimation, const Hash& jumpAnimation, const Hash& endAnimation)
			: _startAnimation(startAnimation), _jumpAnimation(jumpAnimation), _endAnimation(endAnimation) {}

		Hash getStartAnimation(void) const { return _startAnimation; }
		Hash getJumpAnimation(void) const { return _jumpAnimation; }
		Hash getEndAnimation(void) const { return _endAnimation; }

	private:
		Hash _startAnimation;
		Hash _jumpAnimation;
		Hash _endAnimation;

		JumpInfo(const JumpInfo&);
		JumpInfo& operator=(const JumpInfo&);
	};

	typedef std::unordered_map<float, const JumpInfo*> JumpInfoCollection;
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
		float getFarthest(void) const;
		float getHighest(void) const;

		void dispose(void) const;
		
	private:
		JumpInfoCollection _data;

		JumpInfoProvider(void);
		JumpInfoProvider(const JumpInfoProvider&);
		JumpInfoProvider& operator=(const JumpInfoProvider&);
	};

	class JumpHelper
	{
	public:
		JumpHelper(void) : _angle(0.0f), _ledgeDirected(false) {}

		float getAngle(void) const { return _angle; }
		void setAngle(float angle) { _angle = angle; }
		const JumpInfo& getInfo(void) const { return *JumpInfoProvider::get().getData().at(getAngle()); }
		bool isLedgeDirected(void) const { return _ledgeDirected; }
		void setLedgeDirected(bool ledgeDirected) { _ledgeDirected = ledgeDirected; }
	private:
		float _angle;
		bool _ledgeDirected;

		JumpHelper(const JumpHelper&);
		JumpHelper& operator=(const JumpHelper&);
	};

	/**********************************************************************************************
	 * Hang descend helper
	 *********************************************************************************************/
	class HangDescendHelper
	{
	public:
		HangDescendHelper(void) : _point(Point::Zero) {}

		void setPoint(const SensorCollisionParams& params);
		const Point& getPoint(void) const { return _point; }
		void setPoint(const Point& point) { _point = point; }
	private:
		Point _point;

		HangDescendHelper(const HangDescendHelper&);
		HangDescendHelper& operator=(const HangDescendHelper&);
	};

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController(void) : StateMachineComponent(getStates(), "ACT") {}

		ComponentType::Enum getType(void) const { return ComponentType::ACTION_CONTROLLER; }
		JumpHelper& getJumpHelper(void) { return _jumpHelper; }
		HangDescendHelper& getHangDescendHelper(void) { return _hangDescendHelper; }

		void handleMessage(Message& message);

	protected:
		Hash getInitialState(void) const;

	private:
		JumpHelper _jumpHelper;
		HangDescendHelper _hangDescendHelper;

		StateCollection getStates() const;
	};

	/**********************************************************************************************
	 * States
	 *********************************************************************************************/
	class Stand : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class Fall : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class Walk : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;

	private:
		static const float NO_FLOOR_TIME_TO_FALL_IN_MILLIS;
	};

	class Turn : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class PrepareToJump : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;

	private:
		void handleJumpSensor(Message &message) const;
	};

	class JumpStart : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class Jump : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class JumpEnd : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class PrepareToHang : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;

	private:
		void update(void) const;
	};

	class Hanging : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class Hang : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};

	class Drop : public ComponentState
	{
	public:
		void enter(void) const;
		void exit(void) const;
		void handleMessage(Message& message) const;
	};

	class Climb : public ComponentState
	{
	public:
		void enter(void) const;
		void exit(void) const;
		void handleMessage(Message& message) const;
	};

	class PrepareToDescend : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;

	private:
		void update(void) const;
	};

	class Descend : public ComponentState
	{
	public:
		void enter(void) const;
		void handleMessage(Message& message) const;
	};
}

#endif