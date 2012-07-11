#ifndef MESSAGEPARAMS_H
#define MESSAGEPARAMS_H

#include "Hash.h"
#include "NumericPair.h"

namespace Temporal
{
	class ResetAnimationParams
	{
	public:
		explicit ResetAnimationParams(const Hash& animationID, bool rewind = false, bool repeat = false) : _animationID(animationID), _rewind(rewind), _repeat(repeat) {}

		const Hash& getAnimationID(void) const { return _animationID; }
		bool getRewind(void) const { return _rewind; }
		bool getRepeat(void) const { return _repeat; }

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

		const Hash& getSensorID(void) const { return _sensorID; }
		const Point* getPoint(void) const { return _point; }
	
	private:
		const Hash _sensorID;
		const Point* _point;

		SensorCollisionParams(const SensorCollisionParams&);
		SensorCollisionParams& operator=(const SensorCollisionParams&);
	};

	class SceneNodeParams
	{
	public:
		SceneNodeParams(const Hash& sceneNodeID, const Vector& translation, float rotation, const Hash& spriteGroupID, float spriteID)
			: _sceneNodeID(sceneNodeID), _translation(translation), _rotation(rotation), _spriteGroupID(spriteGroupID), _spriteID(spriteID) {}

		const Hash& getSceneNodeID(void) const { return _sceneNodeID; }
		const Vector& getTranslation(void) const { return _translation; }
		float getRotation(void) const { return _rotation; }
		const Hash& getSpriteGroupID(void) const { return _spriteGroupID; }
		float getSpriteID(void) const { return _spriteID; }
	private:
		const Hash _sceneNodeID;
		const Vector _translation;
		const float _rotation;
		const Hash _spriteGroupID;
		const float _spriteID;

		SceneNodeParams(const SceneNodeParams&);
		SceneNodeParams& operator=(const SceneNodeParams&);
	};
}
#endif