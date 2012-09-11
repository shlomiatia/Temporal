#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include "BaseEnums.h"
#include "Hash.h"

namespace Temporal
{
	class Component;
	class Message;
	class Vector;
	class Vector;

	class SensorCollisionParams
	{
	public:
		SensorCollisionParams(Hash sensorID, const Vector* point) : _sensorID(sensorID), _point(point) {}

		Hash getSensorID() const { return _sensorID; }
		const Vector* getPoint() const { return _point; }
	
	private:
		const Hash _sensorID;
		const Vector* _point;

		SensorCollisionParams(const SensorCollisionParams&);
		SensorCollisionParams& operator=(const SensorCollisionParams&);
	};

	const Vector& getPosition(const Component& component);
	Side::Enum getOrientation(const Component& component);

	Hash getHashParam(void* data);
	const SensorCollisionParams& getSensorCollisionParams(void* data);
	const Vector& getVectorParam(void* data);
	inline float getFloatParam(void* data) { return *static_cast<float*>(data); }
	inline int getIntParam(void* data) { return *static_cast<int*>(data); }
	inline bool getBoolParam(void* data) { return *static_cast<bool*>(data); }

	void sendDirectionAction(const Component& component, Side::Enum direction);
	bool isSensorCollisionMessage(Message& message, Hash sensorID);

}
#endif