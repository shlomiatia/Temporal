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
	 * Jump Helpers
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
		JumpHelper() : _type(JumpType::UP) {}

		JumpType::Enum getType() const { return _type; }
		void setType(JumpType::Enum type) { _type = type; }
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


	/**********************************************************************************************
	 * Action controller
	 *********************************************************************************************/
	class ActionController : public StateMachineComponent
	{
	public:
		ActionController();

		Hash getType() const { return TYPE; }
		JumpHelper& getJumpHelper() { return _jumpHelper; }
		void setClimbVector(const Vector& vector) { _climbVector = vector; }
		const Vector& getClimbVector() const { return _climbVector; } 
		void setTakedownEntityId(Hash takedownEntityId) { _takedownEntityId = takedownEntityId; }
		Hash getTakedownEntityId() const { return _takedownEntityId; } 

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
		Vector _climbVector; 
		Hash _takedownEntityId;
		JumpHelper _jumpHelper;
		LedgeDetector _ledgeDetector;

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
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Fall : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Walk : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Slide : public ComponentState
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

		class Descend : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Takedown : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const;
		};

		class Dying : public ComponentState
		{
		public:
			void enter() const;
			void handleMessage(Message& message) const {}
		};
	}
}

#endif