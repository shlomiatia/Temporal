#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Timer.h"
#include "EntitySystem.h"
#include "Hash.h"
#include "Animation.h"
#include <memory>

namespace Temporal
{
	class ResetAnimationParams;
	class SceneNode;

	class SceneNodeBinding
	{
	public:
		SceneNodeBinding(SceneNode& node)
			: _node(node), _index(0) {}

		SceneNode& getSceneNode() { return _node; }
		int getIndex() { return _index; }
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
		 Animator(char* animationSetFile = 0, Hash animationId = Hash::INVALID) :
			_animationSetFile(animationSetFile), _animationId(animationId) {}
		
		ComponentType::Enum getType() const { return ComponentType::ANIMATOR; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile, _animationId); }

	private:
		char* _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeBindingCollection _bindings;
		Hash _animationId;
		Timer _timer;

		friend class SerializationAccess;

		void update(float framePeriod);
		void reset(Hash animationId);
	};
}
#endif