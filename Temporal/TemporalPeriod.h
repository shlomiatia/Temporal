#ifndef TEMPORALPERIOD_H
#define TEMPORALPERIOD_H

#include "EntitySystem.h"

namespace Temporal
{
	namespace Period
	{
		enum Enum
		{
			PAST,
			PRESENT,
			FUTURE
		};
	}

	class PlayerPeriod : public Component
	{
	public:
		explicit PlayerPeriod(Period::Enum period = Period::PRESENT) : _period(period) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }

		Component* clone() const { return new PlayerPeriod(_period); }
		static const Hash TYPE;
	private:
		
		Period::Enum _period;

		void changePeriod(Period::Enum period);

		friend class SerializationAccess;
	};

	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period = Period::PRESENT) : _period(period) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }

		Component* clone() const { return new TemporalPeriod(_period); }
		static const Hash TYPE;
	private:		

		Period::Enum _period;

		friend class SerializationAccess;
	};

	
}
#endif