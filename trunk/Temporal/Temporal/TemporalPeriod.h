#ifndef TEMPORALPERIOD_H
#define TEMPORALPERIOD_H

#include "EntitySystem.h"

namespace Temporal
{
	namespace Period
	{
		enum Enum
		{
			NONE = -1,
			PAST = 0,
			PRESENT = 1,
			FUTURE = 2
		};
	}

	class PlayerPeriod : public Component
	{
	public:
		explicit PlayerPeriod(Period::Enum period = Period::PRESENT) : _period(period) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }
		Component* clone() const { return new PlayerPeriod(_period); }

		void setPeriod(Period::Enum period) { _period = period; }
		Period::Enum getPeriod() const { return _period; }
		void changePeriod(Period::Enum period);

		static const Hash TYPE;
	private:
		Period::Enum _period;

		friend class SerializationAccess;
	};

	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period = Period::PRESENT, Hash futureSelfId = Hash::INVALID) : _period(period), _futureSelfId(futureSelfId) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }
		Component* clone() const { return new TemporalPeriod(_period, _futureSelfId); }

		void setPeriod(Period::Enum period);
		Period::Enum getPeriod() const { return _period; }
		void setFutureSelfId(Hash futureSelfId) { _futureSelfId = futureSelfId; }
		Hash getFutureSelfId() const { return _futureSelfId; }

		static const Hash TYPE;
	private:		

		void temporalPeriodChanged(Period::Enum period);
		Period::Enum _period;
		Hash _futureSelfId;

		friend class SerializationAccess;
	};

	
}
#endif