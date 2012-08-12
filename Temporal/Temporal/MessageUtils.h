#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include "BaseEnums.h"
#include "Hash.h"

namespace Temporal
{
	class Component;
	class Message;
	class Point;
	class Vector;
	class Serialization;

	class ResetAnimationParams
	{
	public:
		explicit ResetAnimationParams(const Hash& animationID, bool rewind = false, bool repeat = false) : _animationID(animationID), _rewind(rewind), _repeat(repeat) {}

		const Hash& getAnimationID() const { return _animationID; }
		bool getRewind() const { return _rewind; }
		bool getRepeat() const { return _repeat; }

	private:
		const Hash _animationID;
		const bool _rewind;
		const bool _repeat;

		ResetAnimationParams(const ResetAnimationParams&);
		ResetAnimationParams& operator=(const ResetAnimationParams&);
	};

	class SensorCollisionParams
	{
	public:
		SensorCollisionParams(const Hash& sensorID, const Point* point) : _sensorID(sensorID), _point(point) {}

		const Hash& getSensorID() const { return _sensorID; }
		const Point* getPoint() const { return _point; }
	
	private:
		const Hash _sensorID;
		const Point* _point;

		SensorCollisionParams(const SensorCollisionParams&);
		SensorCollisionParams& operator=(const SensorCollisionParams&);
	};

	const Point& getPosition(const Component& component);
	Side::Enum getOrientation(const Component& component);

	const Hash& getHashParam(void* data);
	Serialization& getSerializationParam(void* data);
	const Serialization& getConstSerializationParam(void* data);
	const ResetAnimationParams& getResetAnimationParams(void* data);
	const SensorCollisionParams& getSensorCollisionParams(void* data);
	const Vector& getVectorParam(void* data);
	inline float getFloatParam(void* data) { return *static_cast<float*>(data); }
	inline int getIntParam(void* data) { return *static_cast<int*>(data); }
	inline bool getBoolParam(void* data) { return *static_cast<bool*>(data); }

	void sendDirectionAction(const Component& component, Side::Enum direction);
	bool isSensorCollisionMessage(Message& message, const Hash& sensorID);

}
#endif