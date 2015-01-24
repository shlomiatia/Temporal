#include "Animation.h"

namespace Temporal
{
	void Animation::init()
	{
		for(SceneGraphSampleIterator gi1 = getSamples().begin(); gi1 != getSamples().end(); ++gi1)
		{
			SceneGraphSample& sceneGraphSample1 = **gi1;
			for(SceneNodeSampleIterator ni1 = sceneGraphSample1.getSamples().begin(); ni1 != sceneGraphSample1.getSamples().end(); ++ni1)
			{
				SceneNodeSample& sceneNodeSample1 = *ni1->second;
				sceneNodeSample1.setParent(sceneGraphSample1);
				bool isFound = false;
				for(SceneGraphSampleIterator gi2 = gi1 + 1; gi2 != getSamples().end(); ++gi2)
				{
					SceneGraphSample& sceneGraphSample2 = **gi2;
					SceneNodeSampleIterator ni2 = sceneGraphSample2.getSamples().find(sceneNodeSample1.getId());
					if(ni2 != sceneGraphSample2.getSamples().end())
					{
						SceneNodeSample& sceneNodeSample2 = *ni2->second;
						sceneNodeSample1.setNext(&sceneNodeSample2);
						sceneNodeSample2.setPrevious(&sceneNodeSample1);
						isFound = true;
						break;
					}
				}
				if(!isFound)
				{
					SceneGraphSample& firstSceneGraphSample = **getSamples().begin();
					SceneNodeSample& sceneNodeSample2 = *firstSceneGraphSample.getSamples().find(sceneNodeSample1.getId())->second;
					sceneNodeSample1.setNext(&sceneNodeSample2);
					sceneNodeSample2.setPrevious(&sceneNodeSample1);
				}
			}
		}
	}

	void AnimationSet::init()
	{
		for(AnimationIterator i = _animations.begin(); i != _animations.end(); ++i)
		{
			i->second->init();
		}
	}

	void AnimationSet::remove(Hash id)
	{
		AnimationIterator i = _animations.find(id);
		delete i->second;
		_animations.erase(i);
	}

	SceneGraphSample* SceneGraphSample::clone() const
	{
		SceneGraphSample* clone = new SceneGraphSample(_index);
		for(SceneNodeSampleIterator i = getSamples().begin(); i != getSamples().end(); ++i)
		{
			clone->getSamples()[i->first] = i->second->clone();
		}
		return clone;
	}

	Animation* Animation::clone() const
	{
		Animation* clone = new Animation(getId(), _repeat);
		for(SceneGraphSampleIterator i = getSamples().begin(); i != getSamples().end(); ++i)
		{
			clone->getSamples().push_back((**i).clone());
		}
		return clone;
	}

	AnimationSet* AnimationSet::clone() const
	{
		AnimationSet* clone = new AnimationSet();
		for(AnimationIterator i = _animations.begin(); i != _animations.end(); ++i)
		{
			clone->_animations[i->first] = i->second->clone();
		}
		return clone;
	}

	SceneGraphSample::~SceneGraphSample()
	{
		for(SceneNodeSampleIterator i = getSamples().begin(); i != getSamples().end(); ++i)
			delete i->second;
	}

	Animation::~Animation()
	{
		for(SceneGraphSampleIterator i = getSamples().begin(); i != getSamples().end(); ++i)
			delete *i;
	}

	AnimationSet::~AnimationSet()
	{
		for(AnimationIterator i = _animations.begin(); i != _animations.end(); ++i)
			delete i->second;
	}

	int Animation::getDuration() const 
	{ 
		int duration = (**(getSamples().end()-1)).getIndex(); 
		if(isRepeat())
			duration++;
		return duration;
	}
}