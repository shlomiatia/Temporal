#ifndef TEMPORALPERIOD_H
#define TEMPORALPERIOD_H

#include "EntitySystem.h"

namespace Temporal
{
	namespace Period
	{
		enum Enum
		{
			PAST = 1,
			PRESENT = 2,
			FUTURE = 4
		};
	}

	class PlayerPeriod : public Component
	{
	public:
		explicit PlayerPeriod(Period::Enum period = Period::PRESENT) : _period(period) {}

		ComponentType::Enum getType() const { return ComponentType::TEMPORAL_PERIOD; }
		void handleMessage(Message& message);

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("period", (int&)_period);
		}
	private:
		Period::Enum _period;

		void changePeriod(Period::Enum period);
	};

	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period = Period::PRESENT) : _period(period) {}

		ComponentType::Enum getType() const { return ComponentType::TEMPORAL_PERIOD; }
		void handleMessage(Message& message);

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("period", (int&)_period);
		}
	private:
		Period::Enum _period;
	};

	
}
#endif