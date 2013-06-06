#ifndef PATROL_H
#define PATROL_H

#include "StateMachineComponent.h"
#include "Sensor.h"

namespace Temporal
{
	class EdgeDetector : public ContactListener
	{
	public:
		explicit EdgeDetector(Hash sensorId)
			: ContactListener(sensorId), _isFound(false) {}

		bool isFound() const { return _isFound; }

	protected:
		void start();
		void handle(const Contact& contact);

	private:
		bool _isFound;
	};

	class Patrol : public StateMachineComponent
	{
	public:
		Patrol();

		ComponentType::Enum getType() const { return ComponentType::PATROL; }
		Component* clone() const { return new Patrol(); }
		void handleMessage(Message& message);

		const EdgeDetector& getEdgeDetector() const { return _edgeDetector; }

	protected:
		Hash getInitialState() const;

	private:
		EdgeDetector _edgeDetector;

		StateCollection getStates() const;
	};

	namespace PatrolStates
	{
		class Walk : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;
		};

		class See : public ComponentState
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

		class Wait : public ComponentState
		{
		public:
			void handleMessage(Message& message) const;

		private:
			static const float WAIT_TIME;
		};
	}
}


#endif