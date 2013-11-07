#ifndef ANIMATION_H
#define ANIMATION_H

#include "Vector.h"
#include "Hash.h"

namespace Temporal
{
	class SceneGraphSample;

	namespace Direction
	{
		enum Enum
		{
			FORWARD = 1,
			BACKWARD = -1
		};
	}

	class SceneNodeSample
	{
	public:
		explicit SceneNodeSample(Hash sceneNodeId = Hash::INVALID, Hash spriteGroupId = Hash::INVALID, const Vector& translation = Vector::Zero, float rotation = 0.0f) :
		  _sceneNodeId(sceneNodeId), _spriteGroupId(spriteGroupId), _translation(translation), _rotation(rotation), _parent(0), _next(0), _previous(0) {}

		Hash getId() const { return _sceneNodeId; }
		Hash getSpriteGroupId() const { return _spriteGroupId; }
		void setTranslation(const Vector& translation) { _translation = translation; }
		const Vector& getTranslation() const { return _translation; }
		void setRotation(float rotation) { _rotation = rotation; }
		float getRotation() const { return _rotation; }
		const SceneNodeSample* getNext() const { return _next; }
		const SceneNodeSample* getPrevious() const { return _previous; }
		const SceneNodeSample* getSibling(Direction::Enum direction) const { return direction == Direction::FORWARD ? _next : _previous; }
		void setNext(const SceneNodeSample* next) { _next = next; }
		void setPrevious(const SceneNodeSample* next) { _previous = next; }
		const SceneGraphSample& getParent() const { return * _parent; }
		void setParent(SceneGraphSample& parent) { _parent = &parent; }

		SceneNodeSample* clone() const { return new SceneNodeSample(getId(), getSpriteGroupId(),  getTranslation(), getRotation()); }

	private:
		Hash _sceneNodeId;
		Hash _spriteGroupId;
		Vector _translation;
		float _rotation;
		const SceneGraphSample* _parent;
		const SceneNodeSample* _next;
		const SceneNodeSample* _previous;

		SceneNodeSample(const SceneNodeSample&);
		SceneNodeSample& operator=(const SceneNodeSample&);

		friend class SerializationAccess;
	};

	typedef std::unordered_map<Hash, SceneNodeSample*> SceneNodeSampleCollection;
	typedef SceneNodeSampleCollection::const_iterator SceneNodeSampleIterator;
	
	class SceneGraphSample
	{
	public:
		SceneGraphSample(int index = 0) : _index(index) {}
		~SceneGraphSample();

		int getIndex() const { return _index; }
		SceneNodeSampleCollection& getSamples() { return _samples; }
		const SceneNodeSampleCollection& getSamples() const { return _samples; }

		SceneGraphSample* clone() const;
		void init();

	private:
		int _index;
		SceneNodeSampleCollection _samples;

		SceneGraphSample(const SceneGraphSample&);
		SceneGraphSample& operator=(const SceneGraphSample&);

		friend class SerializationAccess;
	};

	typedef std::vector<SceneGraphSample*> SceneGraphSampleCollection;
	typedef SceneGraphSampleCollection::const_iterator SceneGraphSampleIterator;

	class Animation
	{
	public:
		Animation(Hash id = Hash::INVALID, bool repeat = false, bool rewind = false) :
		  _id(id), _repeat(repeat), _rewind(rewind) {}
		~Animation();
		
		int getDuration() const { return(**(getSamples().end()-1)).getIndex() + 1; }
		void setId(Hash id) { _id = id; }
		Hash getId() const { return _id; }
		bool repeat() const { return _repeat; }
		void setRepeat(bool repeat) { _repeat = repeat; }
		bool rewind() const { return _rewind; }
		SceneGraphSampleCollection& getSamples() { return _samples; }
		const SceneGraphSampleCollection& getSamples() const { return _samples; }

		void init();
		Animation* clone() const;

	private:
		Hash _id;
		bool _repeat;
		bool _rewind;
		SceneGraphSampleCollection _samples;

		Animation(const Animation&);
		Animation& operator=(const Animation&);

		friend class SerializationAccess;
	};

	typedef std::unordered_map<Hash, Animation*> AnimationCollection;
	typedef AnimationCollection::const_iterator AnimationIterator;

	class AnimationSet
	{
	public:
		AnimationSet() {}
		~AnimationSet();

		void add(Animation* animation) { _animations[animation->getId()] = animation; }
		void remove(Hash id);
		Animation& get(Hash id) const { if(!_animations.count(id)) id = Hash("POP_ANM_BASE"); return *_animations.at(id); }
		AnimationCollection& getAnimations() { return _animations; }
		const AnimationCollection& getAnimations() const { return _animations; }
		AnimationSet* clone() const;
		void init();
		
	private:
		AnimationCollection _animations;

		AnimationSet(const AnimationSet&);
		AnimationSet& operator=(const AnimationSet&);

		friend class SerializationAccess;
	};
}
#endif