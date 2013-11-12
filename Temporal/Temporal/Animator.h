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
	class AnimationParams;

	class SceneNodeBinding
	{
	public:
		SceneNodeBinding(SceneNode& node)
			: _node(node), _sample(0) {}

		SceneNode& getSceneNode() { return _node; }
		const SceneNodeSample* getSample() { return _sample; }
		void setSample(const SceneNodeSample* sample) { _sample = sample; }

	private:
		SceneNode& _node;	
		const SceneNodeSample* _sample;
	};

	typedef std::vector<SceneNodeBinding*> SceneNodeBindingCollection;
	typedef SceneNodeBindingCollection::const_iterator SceneNodeBindingIterator;

	class Animator : public Component
	{
	public:
		Animator(const char* animationSetFile = "", Hash animationId = Hash::INVALID) :
			_animationSetFile(animationSetFile), _animationId(animationId), _isPaused(false), _isRewined(false) {}
		~Animator();
		
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new Animator(_animationSetFile.c_str(), _animationId); }

		static const Hash TYPE;
	private:
		static const int FPS;

		std::string _animationSetFile;
		std::shared_ptr<AnimationSet> _animationSet;
		SceneNodeBindingCollection _bindings;
		Hash _animationId;
		Timer _timer;
		bool _isPaused;
		bool _isRewined;

		friend class SerializationAccess;

		void update();
		void reset(AnimationParams& animationParams);
	};
}
#endif