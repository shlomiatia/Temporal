#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include "BaseEnums.h"
#include "Hash.h"

namespace Temporal
{
	class Component;
	class Message;
	class YABP;
	class Vector;

	class LedgeDetectionParams
	{
	public:
		LedgeDetectionParams(Hash sensorID, const YABP* platform) : _sensorID(sensorID), _platform(platform) {}

		Hash getSensorID() const { return _sensorID; }
		const YABP* getPlatform() const { return _platform; }
	
	private:
		const Hash _sensorID;
		const YABP* _platform;

		LedgeDetectionParams(const LedgeDetectionParams&);
		LedgeDetectionParams& operator=(const LedgeDetectionParams&);
	};

	const Vector& getPosition(const Component& component);
	Side::Enum getOrientation(const Component& component);

	Hash getHashParam(void* data);
	const LedgeDetectionParams& getLedgeDetectionParams(void* data);
	const Vector& getVectorParam(void* data);
	inline float getFloatParam(void* data) { return *static_cast<float*>(data); }
	inline int getIntParam(void* data) { return *static_cast<int*>(data); }
	inline bool getBoolParam(void* data) { return *static_cast<bool*>(data); }

	void sendDirectionAction(const Component& component, Side::Enum direction);
	bool isSensorCollisionMessage(Message& message, Hash sensorID);

}
#endif