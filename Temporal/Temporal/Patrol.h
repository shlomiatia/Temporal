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
		Patrol(Hash securityCameraId = Hash::INVALID, bool isStatic = false, Hash initialStateId = Hash::INVALID);

		Hash getType() const { return ComponentsIds::PATROL; }
		Component* clone() const { return new Patrol(_securityCameraId, _isStatic, getCurrentStateID()); }
		void handleMessage(Message& message);
		bool isStatic() const { return _isStatic; }
		bool handleAlarmMessage(Message& message);
		bool handleFireMessage(Message& message);
		bool handleTakedownMessage(Message& message);
	protected:
		Hash getInitialState() const;

	private:
		bool _isStatic;
		Hash _securityCameraId;
		EdgeDetector _edgeDetector;

		HashStateMap getStates() const;

		friend class SerializationAccess;
	};

	namespace PatrolStates
	{
		class Walk : public ComponentState
		{
		public:
			void handleMessage(Message& message);
		};

		class Aim : public ComponentState
		{
		public:
			void enter(void* param);
			void handleMessage(Message& message);

		private:
			static const float AIM_TIME;
		};

		class Fire : public ComponentState
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
			void enter(void* param);
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

		class Dead : public ComponentState
		{
		public:
			void handleMessage(Message& message) {};
		};
	}
}


#endif