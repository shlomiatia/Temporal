#ifndef ANIMATIONSAMPLE_H
#define ANIMATIONSAMPLE_H

#include <vector>

namespace Temporal
{
	class SceneNodeSample;

	typedef std::vector<const SceneNodeSample*> SceneNodeSampleCollection;
	typedef SceneNodeSampleCollection::const_iterator SceneNodeSampleIterator;

	class AnimationSample
	{
	public:
		AnimationSample(float duration = 0.0f) : _duration(duration) {}

		float getDuration(void) const { return _duration; }

		void add(const SceneNodeSample* node) { _sceneNodeSamples.push_back(node); }
	private:
		float _duration;
		SceneNodeSampleCollection _sceneNodeSamples;

		AnimationSample(const AnimationSample&);
		AnimationSample& operator=(const AnimationSample&);
	};
}
#endif