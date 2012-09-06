#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Timer.h"
#include "EntitySystem.h"
#include "Hash.h"
#include "Animation.h"

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
		 Animator(Hash animationSetId = Hash::INVALID, Hash animationId = Hash::INVALID) :
			_animationSetId(animationSetId), _animationSet(NULL), _animationId(animationId) {}
		
		ComponentType::Enum getType() const { return ComponentType::ANIMATOR; }
		void handleMessage(Message& message);
		Component* clone() const;

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("animation-set", _animationSetId);
			serializer.serialize("animation", _animationId);
		}
	private:
		Hash _animationSetId;
		const AnimationSet* _animationSet;
		SceneNodeBindingCollection _bindings;
		Hash _animationId;
		Timer _timer;

		void init();
		void update(float framePeriodInMillis);
		void reset(Hash animationId);
	};
}
#endif