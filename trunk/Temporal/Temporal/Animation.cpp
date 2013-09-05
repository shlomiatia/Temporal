#include "Animation.h"
#include <algorithm>

namespace Temporal
{
	void SampleSet::init()
	{
		_duration = 0.0f;
		for(SampleIterator i = _samples.begin(); i != _samples.end(); ++i)
		{
			Sample& sample = **i;
			if(i != _samples.begin())
			{
				sample.setStartTime((**(i - 1)).getEndTime());
			}
			_duration += sample.getDuration();
		
		}
	}
	
	void Animation::init()
	{
		_duration = 0.0f;
		for(SampleSetIterator i = _sampleSets.begin(); i != _sampleSets.end(); ++i)
		{
			i->second->init();
			_duration = std::max(_duration, i->second->getDuration());
		}
	}

	void AnimationSet::init()
	{
		for(AnimationIterator i = _animations.begin(); i != _animations.end(); ++i)
		{
			i->second->init();
		}
	}
}