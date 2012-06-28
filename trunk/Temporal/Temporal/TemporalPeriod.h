#ifndef TEMPORALPERIOD_H
#define TEMPORALPERIOD_H

#include "Component.h"

namespace Temporal
{
	namespace Period
	{
		enum Enum
		{
			Past = 1,
			Present = 2,
			Future = 4
		};
	}

	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period) : _period(period) {}

		ComponentType::Enum getType(void) const { return ComponentType::TEMPORAL_PERIOD; }
		void handleMessage(Message& message);
	private:
		Period::Enum _period;
	};
}
#endif