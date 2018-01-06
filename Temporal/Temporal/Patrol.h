#pragma once
#include "StateMachineComponent.h"
#include "Ids.h"
#include "EdgeDetector.h"

namespace Temporal
{
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
}
