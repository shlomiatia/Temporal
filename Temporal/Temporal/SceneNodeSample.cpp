#include "SceneNodeSample.h"

namespace Temporal
{
	void Animation::add(const Hash& sceneNodeID, float duration, const Vector& translation, float rotation)
	{
		SceneNodeSampleCollection* sceneNodeAnimation = (SceneNodeSampleCollection*)_sceneGraphSamples[sceneNodeID];
		if(sceneNodeAnimation == NULL)
		{
			sceneNodeAnimation = new SceneNodeSampleCollection();
			_sceneGraphSamples[sceneNodeID] = sceneNodeAnimation;
		}
		float startTime = 0.0f;
		int size = sceneNodeAnimation->size();
		if(size > 0)
			startTime = sceneNodeAnimation->at(size - 1)->getEndTime();
		SceneNodeSample* sceneNodeSample = new SceneNodeSample(sceneNodeID, startTime, duration, translation, rotation);
		sceneNodeAnimation->push_back(sceneNodeSample);
		float jointAnimationDuration = startTime + duration;
		if(_duration < jointAnimationDuration)
			_duration = jointAnimationDuration;
	}
}