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
	private:
		Period::Enum _period;

		void changePeriod(Period::Enum period);

		friend class SerializationAccess;
	};

	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period = Period::PRESENT) : _period(period) {}

		ComponentType::Enum getType() const { return ComponentType::TEMPORAL_PERIOD; }
		void handleMessage(Message& message);
	private:
		Period::Enum _period;

		friend class SerializationAccess;
	};

	
}
#endif