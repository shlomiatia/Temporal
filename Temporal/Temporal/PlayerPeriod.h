#ifndef PLAYERPERIOD_H
#define PLAYERPERIOD_H

#include "EntitySystem.h"
#include "Color.h"
#include "ScriptsEnums.h"
#include "Ids.h"

namespace Temporal
{
	typedef std::vector<Color> ColorList;
	typedef ColorList::const_iterator ColorIterator;

	/**********************************************************************************************
	* Player Period
	*********************************************************************************************/
	class PlayerPeriod : public Component
	{
	public:
		explicit PlayerPeriod(Period::Enum period = Period::PRESENT) : _period(period) { _colorIterator = COLORS.begin(); }

		void handleMessage(Message& message);
		Hash getType() const { return ComponentsIds::PLAYER_PERIOD; }
		Component* clone() const { return new PlayerPeriod(_period); }

		void setPeriod(Period::Enum period) { _period = period; }
		Period::Enum getPeriod() const { return _period; }
		const Color& getNextColor();
		void changePeriod(Period::Enum period);

		
	private:
		Period::Enum _period;
		ColorIterator _colorIterator;

		static const ColorList COLORS;

		friend class SerializationAccess;
	};
}

#endif