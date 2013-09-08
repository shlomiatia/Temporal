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
		explicit LedgeDetector(Hash sensorId, MessageID::Enum messageId)
			: ContactListener(sensorId), _messageId(messageId), _isFailed(false), _isFound(false) {}

	protected:
		void start();
		void handle(const Contact& contact);
		void end(Component& component);

	private:
		const MessageID::Enum _messageId;
		bool _isFound;
		bool _isFailed;
	};

	class LedgeDetector2 : public ContactListener
	{
	public:
		explicit LedgeDetector2(Hash sensorId, MessageID::Enum messageId)
			: ContactListener(sensorId), _messageId(messageId), _isFailed(false), _height(-1.0f) {}

	protected:
		void start();
		void handle(const Contact& contact);
		void end(Component& component);

	private:
		float _height;
		const MessageID::Enum _messageId;
		bool _isFailed;
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

		void handleMessage(Message& message);
		Component* clone() const { return new ActionController(); }

		static float WALK_ACC_PER_SECOND;
		static float MAX_WALK_FORCE_PER_SECOND;
		static float JUMP_FORCE_PER_SECOND;
		static float FALL_ALLOW_JUMP_TIME;
		static float JUMP_STOP_MODIFIER;
		static float MAX_WALK_JUMP_MODIFIER;
		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:

		JumpHelper _jumpHelper;
		LedgeDetector _hangDetector;
		LedgeDetector2 _hangDetector2;
		LedgeDetector _descendDetector;

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
	}
}

#endif