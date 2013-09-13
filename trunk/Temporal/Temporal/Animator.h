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
		 Animator(const char* animationSetFile = "", Hash animationId = Hash::INVALID) :
			_animationSetFile(animationSetFile), _animationId(animationId), _isPaused(false) {}
		
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile.c_str(), _animationId); }

		static const Hash TYPE;
	private:
		std::string _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeBindingCollection _bindings;
		Hash _animationId;
		Timer _timer;
		bool _isPaused;

		friend class SerializationAccess;

		void update();
		void reset(Hash animationId);
	};
}
#endif