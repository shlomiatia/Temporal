#ifndef PATROL_H
#define PATROL_H

#include "StateMachineComponent.h"
#include "Sensor.h"

namespace Temporal
{
	class EdgeDetector : public ContactListener
	{
	public:
		explicit EdgeDetector(Hash sensorId, Component& owner)
			: ContactListener(sensorId, owner), _isFound(false) {}

	protected:
		void start();
		void handle(const Contact& contact);
		void end();

	private:
		bool _isFound;
	};

	class Patrol : public StateMachineComponent
	{
	public:
		Patrol(bool isStatic = false);

		Hash getType() const { return TYPE; }
		Component* clone() const { return new Patrol(_isStatic); }
		void handleMessage(Message& message);

		static const Hash TYPE;

		bool isStatic() const { return _isStatic; }
		void setStatic(bool isStatic) { _isStatic = isStatic; }

		void handleWaitWalkMessage(Message& message);
	protected:
		Hash getInitialState() const;

	private:
		bool _isStatic;
		EdgeDetector _edgeDetector;

		HashStateMap getStates() const;

		friend class SerializationAccess;
	};

	namespace PatrolStates
	{
		class Walk : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Acquire : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);

		private:
			static const float ACQUIRE_TIME;
		};

		class See : public ComponentState
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

		class Wait : public ComponentState
		{
		public:
			void handleMessage(Message& message);

		private:
			static const float WAIT_TIME;
		};

		class Takedown : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);
		};

		class Navigate : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};
	}
}


#endif