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

	SampleSet* SampleSet::clone() const
	{
		SampleSet* clone = new SampleSet(getId(), getDuration());
		for(SampleIterator i = getSamples().begin(); i != getSamples().end(); ++i)
		{
			clone->getSamples().push_back((**i).clone());
		}
		return clone;
	}

	Animation* Animation::clone() const
	{
		Animation* clone = new Animation(getId(), getDuration(), _repeat, _rewind);
		for(SampleSetIterator i = getSampleSets().begin(); i != getSampleSets().end(); ++i)
		{
			clone->getSampleSets()[i->first] = i->second->clone();
		}
		return clone;
	}

	AnimationSet* AnimationSet::clone() const
	{
		AnimationSet* clone = new AnimationSet();
		for(AnimationIterator i = get().begin(); i != get().end(); ++i)
		{
			clone->get()[i->first] = i->second->clone();
		}
		return clone;
	}

	SampleSet::~SampleSet()
	{
		for(SampleIterator i = getSamples().begin(); i != getSamples().end(); ++i)
			delete *i;
	}

	Animation::~Animation()
	{
		for(SampleSetIterator i = getSampleSets().begin(); i != getSampleSets().end(); ++i)
			delete i->second;
	}

	AnimationSet::~AnimationSet()
	{
		for(AnimationIterator i = get().begin(); i != get().end(); ++i)
			delete i->second;
	}
}