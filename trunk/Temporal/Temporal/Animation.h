#ifndef ANIMATION_H
#define ANIMATION_H

#include "Vector.h"
#include "Hash.h"

namespace Temporal
{
	class Sample
	{
	public:
		explicit Sample(Hash spriteGroupId = Hash::INVALID, float startTime = 0.0f, float duration = 0.0f, const Vector& translation = Vector::Zero, float rotation = 0.0f) :
		  _spriteGroupId(spriteGroupId), _translation(translation), _rotation(rotation), _startTime(startTime), _duration(duration) {}

		Hash getSpriteGroupId() const { return _spriteGroupId; }
		void setStartTime(float startTime) { _startTime = startTime; }
		float getStartTime() const { return _startTime; }
		float getEndTime() const { return getStartTime() + getDuration(); }
		void setDuration(float duration) { _duration = duration; }
		float getDuration() const { return _duration; }
		void setTranslation(const Vector& translation) { _translation = translation; }
		const Vector& getTranslation() const { return _translation; }
		void setRotation(float rotation) { _rotation = rotation; }
		float getRotation() const { return _rotation; }

	private:
		Hash _spriteGroupId;
		Vector _translation;
		float _rotation;
		float _startTime;
		float _duration;

		Sample(const Sample&);
		Sample& operator=(const Sample&);

		friend class SerializationAccess;
	};

	typedef std::vector<Sample*> SampleCollection;
	typedef SampleCollection::const_iterator SampleIterator;
	
	class SampleSet
	{
	public:
		SampleSet() : _sceneNodeId(Hash::INVALID), _duration(0.0f) {}

		Hash getId() const { return _sceneNodeId; }
		float getDuration() const { return _duration; }
		SampleCollection& getSamples() { return _samples; }
		const SampleCollection& getSamples() const { return _samples; }
		void init();

	private:
		Hash _sceneNodeId;
		SampleCollection _samples;
		float _duration;

		SampleSet(const SampleSet&);
		SampleSet& operator=(const SampleSet&);

		friend class SerializationAccess;
	};

	typedef std::unordered_map<Hash, SampleSet*> SampleSetCollection;
	typedef SampleSetCollection::const_iterator SampleSetIterator;

	class Animation
	{
	public:
		Animation() : _id(Hash::INVALID), _duration(0.0f), _repeat(false), _rewind(false) {}
		
		Hash getId() const { return _id; }
		float getDuration() const { return _duration; }
		bool Repeat() const { return _repeat; }
		bool Rewind() const { return _rewind; }
		const SampleSet& get(Hash sceneNodeID) const { return *_sampleSets.at(sceneNodeID); }
		SampleSetCollection& getSampleSets() { return _sampleSets; }

		void init();

	private:
		Hash _id;
		bool _repeat;
		bool _rewind;
		SampleSetCollection _sampleSets;
		float _duration;

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

		void add(Animation* animation) { _animations[animation->getId()] = animation; }
		const Animation& get(Hash id) const { if(!_animations.count(id)) id = Hash("POP_ANM_BASE"); return *_animations.at(id); }
		AnimationCollection& get() { return _animations; }
		void init();
		
	private:
		AnimationCollection _animations;

		AnimationSet(const AnimationSet&);
		AnimationSet& operator=(const AnimationSet&);

		friend class SerializationAccess;
	};
}
#endif