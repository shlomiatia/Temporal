#ifndef SCENENODESAMPLE_H
#define SCENENODESAMPLE_H

#include "NumericPair.h"
#include "Hash.h"

namespace Temporal
{
	class SceneNodeSample;

	typedef std::vector<const SceneNodeSample*> SceneNodeSampleCollection;
	typedef SceneNodeSampleCollection::const_iterator SceneNodeSampleIterator;
	typedef std::unordered_map<const Hash, const SceneNodeSampleCollection*> SceneGraphSampleCollection;
	typedef SceneGraphSampleCollection::const_iterator SceneGraphSampleIterator;
	typedef std::unordered_map<const Hash, const SceneGraphSampleCollection*> AnimationCollection;
	typedef AnimationCollection::const_iterator AnimationIterator;

	class SceneNodeSample
	{
	public:
		SceneNodeSample(const Hash& spriteGroupId, float duration = 0.0f) :
		  /*_translation(translation), _rotation(rotation), */_spriteGroupId(spriteGroupId), _duration(duration) {}

		float getDuration(void) const { return _duration; }
		Hash getSpriteGroupId(void) const { return _spriteGroupId; }
	private:
		//const Vector _translation;
		//const float _rotation;
		const Hash _spriteGroupId;
		const float _duration;

		SceneNodeSample(const SceneNodeSample&);
		SceneNodeSample& operator=(const SceneNodeSample&);
	};
}
#endif