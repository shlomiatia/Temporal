#ifndef SCENENODESAMPLE_H
#define SCENENODESAMPLE_H

#include "NumericPair.h"
#include "Hash.h"

namespace Temporal
{
	class SceneNodeSample;
	class Animation;

	typedef std::vector<const SceneNodeSample*> SceneNodeSampleCollection;
	typedef SceneNodeSampleCollection::const_iterator SceneNodeSampleIterator;
	typedef std::unordered_map<const Hash, const SceneNodeSampleCollection*> SceneGraphSampleCollection;
	typedef SceneGraphSampleCollection::const_iterator SceneGraphSampleIterator;
	typedef std::unordered_map<const Hash, const Animation*> AnimationCollection;
	typedef AnimationCollection::const_iterator AnimationIterator;

	class SceneNodeSample
	{
	public:
		SceneNodeSample(const Hash& spriteGroupId, float startTime = 0.0f, float duration = 0.0f, const Vector& translation = Vector::Zero, float rotation = 0.0f) :
		  _translation(translation), _rotation(rotation), _spriteGroupId(spriteGroupId), _startTime(startTime), _duration(duration) {}

		float getStartTime(void) const { return _startTime; }
		float getEndTime(void) const { return getStartTime() + getDuration(); }
		float getDuration(void) const { return _duration; }
		const Vector& getTranslation(void) const { return _translation; }
		float getRotation(void) const { return _rotation; }
		Hash getSpriteGroupId(void) const { return _spriteGroupId; }
	private:
		const Vector _translation;
		const float _rotation;
		const Hash _spriteGroupId;
		const float _startTime;
		const float _duration;

		SceneNodeSample(const SceneNodeSample&);
		SceneNodeSample& operator=(const SceneNodeSample&);
	};

	class Animation
	{
	public:
		Animation(void) : _duration(0.0f) {}
		
		float getDuration(void) const { return _duration; }
		void add(const Hash& sceneNodeID, float duration = 0.0f, const Vector& translation = Vector::Zero, float rotation = 0.0f);
		const SceneNodeSampleCollection& get(const Hash& sceneNodeID) const { return *_sceneGraphSamples.at(sceneNodeID); }

	private:
		SceneGraphSampleCollection _sceneGraphSamples;
		float _duration;
	};
}
#endif