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
		float getDuration() const { return _duration; }
		const Vector& getTranslation() const { return _translation; }
		float getRotation() const { return _rotation; }

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("sprite-group", _spriteGroupId);
			serializer.serialize("translation", _translation);
			serializer.serialize("rotation", _rotation);
			serializer.serialize("duration", _duration);
		}
	private:
		Hash _spriteGroupId;
		Vector _translation;
		float _rotation;
		float _startTime;
		float _duration;

		Sample(const Sample&);
		Sample& operator=(const Sample&);
	};

	typedef std::vector<const Sample*> SampleCollection;
	typedef SampleCollection::const_iterator SampleIterator;
	
	class SampleSet
	{
	public:
		SampleSet() : _sceneNodeId(Hash::INVALID), _duration(0.0f) {}

		Hash getSceneNodeId() const { return _sceneNodeId; }
		float getDuration() const { return _duration; }
		void add(Sample* sample);
		const SampleCollection& get() const { return _samples; }

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("scene-node-id", _sceneNodeId);
		}
	private:
		Hash _sceneNodeId;
		SampleCollection _samples;
		float _duration;

		SampleSet(const SampleSet&);
		SampleSet& operator=(const SampleSet&);
	};

	typedef std::unordered_map<Hash, const SampleSet*> SampleSetCollection;
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
		void add(const SampleSet* sampleSet);

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("id", _id);
			serializer.serialize("repeat", _repeat);
			serializer.serialize("rewind", _rewind);
		}
	private:
		Hash _id;
		bool _repeat;
		bool _rewind;
		SampleSetCollection _sampleSets;
		float _duration;

		Animation(const Animation&);
		Animation& operator=(const Animation&);
	};

	typedef std::unordered_map<Hash, const Animation*> AnimationCollection;
	typedef AnimationCollection::const_iterator AnimationIterator;

	class AnimationSet
	{
	public:
		AnimationSet() : _id(Hash::INVALID) {}

		Hash getId() const { return _id; }
		void add(const Animation* animation) { _animations[animation->getId()] = animation; }
		const Animation& get(Hash id) const { return *_animations.at(id); }

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("id", _id);
		}
	private:
		Hash _id;
		AnimationCollection _animations;

		AnimationSet(const AnimationSet&);
		AnimationSet& operator=(const AnimationSet&);
	};
}
#endif