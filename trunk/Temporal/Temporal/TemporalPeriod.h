#ifndef TEMPORALPERIOD_H
#define TEMPORALPERIOD_H

#include "EntitySystem.h"
#include "Vector.h"
#include "ScriptsEnums.h"
#include "Ids.h"

namespace Temporal
{
	class TemporalPeriod : public Component
	{
	public:
		explicit TemporalPeriod(Period::Enum period = Period::PRESENT, Hash futureSelfId = Hash::INVALID, bool createFutureSelf = false, bool syncFutureSelf = false) :
			_period(period), _futureSelfId(futureSelfId), _createFutureSelf(createFutureSelf), _syncFutureSelf(syncFutureSelf), _isMoving(false){}

		void handleMessage(Message& message);
		Hash getType() const { return ComponentsIds::TEMPORAL_PERIOD; }
		Component* clone() const { return new TemporalPeriod(_period, Hash::INVALID, _createFutureSelf); }

		void setPeriod(Period::Enum period);
		Period::Enum getPeriod() const { return _period; }
		Hash getFutureSelfId() const { return _futureSelfId; }

	private:		
		void temporalPeriodChanged(Period::Enum period);
		Period::Enum _period;
		Hash _futureSelfId;
		Hash _editorFutureSelfId;
		bool _createFutureSelf;
		bool _syncFutureSelf;
		Vector _previousFramePosition;
		bool _isMoving;

		void entityReady();
		void die(bool temporalDeath);
		void endMovement(Vector presentPosition);
		void startMovement();
		void update();
		void setImpulse();
		void createFuture();
		void destroyFuture();
		void addParticleEmitter(Entity& entity);

		friend class SerializationAccess;
	};
}
#endif