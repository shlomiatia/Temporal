#ifndef TEMPORALPERIOD_H
#define TEMPORALPERIOD_H

#include "EntitySystem.h"
#include "Color.h"
#include "Vector.h"

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
		Hash getType() const { return TYPE; }
		Component* clone() const { return new PlayerPeriod(_period); }

		void setPeriod(Period::Enum period) { _period = period; }
		Period::Enum getPeriod() const { return _period; }
		const Color& getNextColor();
		void changePeriod(Period::Enum period);

		static const Hash TYPE;
	private:
		Period::Enum _period;
		ColorIterator _colorIterator;

		static const ColorList COLORS;

		friend class SerializationAccess;
	};

	/**********************************************************************************************
	* Temporal Period
	*********************************************************************************************/
	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period = Period::PRESENT, Hash futureSelfId = Hash::INVALID, bool createFutureSelf = false) : 
			_period(period), _futureSelfId(futureSelfId), _createFutureSelf(createFutureSelf) {}

		void handleMessage(Message& message);
		Hash getType() const { return TYPE; }
		Component* clone() const { return new TemporalPeriod(_period, Hash::INVALID, _createFutureSelf); }

		void setPeriod(Period::Enum period);
		Period::Enum getPeriod() const { return _period; }
		void setFutureSelfId(Hash futureSelfId) { _futureSelfId = futureSelfId; }
		Hash getFutureSelfId() const { return _futureSelfId; }
		void setCreateFutureSelf(bool createFutureSelf) { _createFutureSelf = createFutureSelf; }
		bool isCreateFutureSelf() const { return _createFutureSelf; }

		static const Hash TYPE;
	private:		
		void temporalPeriodChanged(Period::Enum period);
		Period::Enum _period;
		Hash _futureSelfId;
		bool _createFutureSelf;
		Vector _previousPosition;

		void createFuture();
		void destroyFuture();
		void addParticleEmitter(Entity& entity);

		friend class SerializationAccess;
	};

	/**********************************************************************************************
	* Temporal Notification
	*********************************************************************************************/
	class TemporalNotification : public Component
	{
	public:
		TemporalNotification() : Component(true) {}

		void handleMessage(Message& message);
		Hash getType() const { return Hash::INVALID; }
		Component* clone() const { return 0; }
	};
}
#endif