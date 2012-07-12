#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Timer.h"
#include "Component.h"
#include "Hash.h"
#include "SceneNodeSample.h"

namespace Temporal
{
	class ResetAnimationParams;

	class SceneNode;

	class SceneNodeBinding
	{
	public:
		SceneNodeBinding(SceneNode& node)
			: _node(node), _index(0) {}

		SceneNode& getSceneNode(void) { return _node; }
		int getIndex(void) { return _index; }
		void setIndex(int index) { _index = index; }

	private:
		SceneNode& _node;	
		int _index;
	};

	typedef std::unordered_map<Hash, SceneNodeBinding*> SceneNodeBindingCollection;
	typedef SceneNodeBindingCollection::const_iterator SceneNodeBindingIterator;

	class Animator : public Component
	{
	public:
		explicit Animator(const AnimationCollection& animations, SceneNode& root);
		
		ComponentType::Enum getType(void) const { return ComponentType::ANIMATOR; }

		void handleMessage(Message& message);
		void update(float framePeriodInMillis);

	private:
		const AnimationCollection& _animations;

		SceneNodeBindingCollection _bindings;
		Hash _animationId;
		bool _rewind;
		bool _repeat;
		Timer _timer;

		void reset(const ResetAnimationParams& resetAnimationParams);
	};
}
#endif