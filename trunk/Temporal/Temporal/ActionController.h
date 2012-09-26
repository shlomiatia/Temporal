#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include "Vector.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "StateMachineComponent.h"

namespace Temporal
{
	class SensorCollisionParams;

	// BRODER
	static const float WALK_FORCE_PER_SECOND = 250.0f;
	static const float JUMP_FORCE_PER_SECOND = 450.0f;

	/**********************************************************************************************
	 * Jump Helpers
	 *********************************************************************************************/
	class JumpInfo
	{
	public:
		JumpInfo(Hash startAnimation, Hash jumpAnimation, Hash endAnimation)
			: _startAnimation(startAnimation), _jumpAnimation(jumpAnimation), _endAnimation(endAnimation) {}

		Hash getStartAnimation() const { return _startAnimation; }
		Hash getJumpAnimation() const { return _jumpAnimation; }
		Hash getEndAnimation() const { return _endAnimation; }

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
		static const JumpInfoProvider& get()
		{
			static JumpInfoProvider instance;
			return instance;
		}

		const JumpInfoCollection& getData() const { return _data; }
		float getFarthest() const;
		float getHighest() const;

		void dispose() const;
		
	private:
		JumpInfoCollection _data;

		JumpInfoProvider();
		JumpInfoProvider(const JumpInfoProvider&);
		JumpInfoProvider& operator=(const JumpInfoProvider&);
	};

	class JumpHelper
	{
	public:
		JumpHelper() : _angle(0.0f), _ledgeDirected(false) {}

		float getAngle() const { return _angle; }
		void setAngle(float angle) { _angle = angle; }
		const JumpInfo& getInfo() const { return *JumpInfoProvider::get().getData().at(getAngle()); }
		bool isLedgeDirected() const { return _ledgeDirected; }
		void setLedgeDirected(bool ledgeDirected) { _ledgeDirected = ledgeDirected; }
	private:
		float _angle;
		bool _ledgeDirected;

		JumpHelper(const JumpHelper&);
		JumpHelper& operator=(const JumpHelper&);

		friend class SerializationAccess;
	};

	/**********************************************************************************************
	 * Hang descend helper
	 *********************************************************************************************/
	class HangDescendHelper
	{
	public:
		HangDescendHelper() : _point(Vector::Zero) {}

		void setPoint(const SensorCollisionParams& params);
		const Vector& getPoint() const { return _point; }
		void setPoint(const Vector& point) { _point = point; }
	private:
		Vector _point;

		HangDescendHelper(const HangDescendHelper&);
		HangDescendHelper& operator=(const HangDescendHelper&);

		friend class SerializationAccess;
	};

	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController() : StateMachineComponent(getStates(), "ACT") {}

		ComponentType::Enum getType() const { return ComponentType::ACTION_CONTROLLER; }
		JumpHelper& getJumpHelper() { return _jumpHelper; }
		HangDescendHelper& getHangDescendHelper() { return _hangDescendHelper; }

		Component* clone() const { return new ActionController(); }

	protected:
		Hash getInitialState() const;

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
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class Fall : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;

	private:
		static const float ALLOW_JUMP_TIME;
	};

	class Walk : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class Turn : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class PrepareToJump : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;

	private:
		void handleJumpSensor(Message &message) const;
	};

	class JumpStart : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class Jump : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class JumpEnd : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class PrepareToHang : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;

	private:
		void update() const;
	};

	class Hang : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};

	class Climb : public ComponentState
	{
	public:
		void enter() const;
		void exit() const;
		void handleMessage(Message& message) const;
	};

	class PrepareToDescend : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;

	private:
		void update() const;
	};

	class Descend : public ComponentState
	{
	public:
		void enter() const;
		void handleMessage(Message& message) const;
	};
}

#endif