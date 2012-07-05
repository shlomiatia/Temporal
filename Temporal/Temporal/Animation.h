#ifndef ANIMATION_H
#define ANIMATION_H

#include "Hash.h"
#include <vector>

namespace Temporal
{
	class AnimationSample;

	typedef std::vector<const AnimationSample*> AnimationSampleCollection;
	typedef AnimationSampleCollection::const_iterator AnimationSampleIterator;

	class Animation
	{
	public:
		Animation(void) {}
		void add(const AnimationSample* animationSample) { _animationSamples.push_back(animationSample); }
		const AnimationSample& get(int index) const { return *_animationSamples[index]; }
	private:
		AnimationSampleCollection _animationSamples;

		Animation(const Animation&);
		Animation& operator=(const Animation&);
	};
}
#endif