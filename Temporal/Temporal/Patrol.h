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
		Patrol(Hash securityCameraId = Hash::INVALID, bool isStatic = false, float waitTime = 1.0f, float aimTime = 0.5f, Hash initialStateId = Hash::INVALID);

		Hash getType() const { return ComponentsIds::PATROL; }
		Component* clone() const { return new Patrol(_securityCameraId, _isStatic, _waitTime, _aimTime, getCurrentStateID()); }
		void handleMessage(Message& message);

		bool isStatic() const { return _isStatic; }
		float getWaitTime() const { return _waitTime; }
		float getAimTime() const { return _aimTime; }

		bool handleAlarmMessage(Message& message);
		bool handleFireMessage(Message& message);
		bool handleTakedownMessage(Message& message);
	protected:
		Hash getInitialState() const;

	private:
		bool _isStatic;
		Hash _securityCameraId;
		float _waitTime;
		float _aimTime;
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