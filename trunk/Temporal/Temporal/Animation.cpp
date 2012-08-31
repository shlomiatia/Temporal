#include "Animation.h"
#include <algorithm>

namespace Temporal
{
	void SampleSet::add(Sample* sample)
	{
		float startTime = 0.0f;
		int size = _samples.size();
		if(size > 0)
			startTime = _samples[size - 1]->getEndTime();
		sample->setStartTime(startTime);
		_samples.push_back(sample);
		_duration += sample->getDuration();
	}
	
	void Animation::add(const SampleSet* sampleSet)
	{
		_duration = std::max(_duration, sampleSet->getDuration());
		_sampleSets[sampleSet->getSceneNodeId()] = sampleSet;
	}
}