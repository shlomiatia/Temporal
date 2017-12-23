#ifndef TEMPORAL_PERIOD_EDITOR_H
#define TEMPORAL_PERIOD_EDITOR_H

#include "ComponentEditor.h"

namespace Temporal
{
	class TemporalPeriod;

	class TemporalPeriodEditor : public ComponentEditor
	{
	public:
		TemporalPeriodEditor(TemporalPeriod& period) : _period(period) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		TemporalPeriod& _period;

		void futureSelfIdChanged(const char* s);
		void createFutureSelfChanged(bool b);
		void syncFutureSelfChanged(bool b);
	};
}

#endif