#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include "BaseEnums.h"
#include "Hash.h"

namespace Temporal
{
	class Entity;
	class Component;
	class Message;
	class OBB;
	class Vector;
	class Contact;
	class Fixture;
	class Entity;
	class RayCastResult;

	typedef std::vector<const Fixture*> FixtureList;
	typedef FixtureList::const_iterator FixtureIterator;
	
	namespace MouseButton { enum Enum; }

	const Vector& getPosition(const Component& component);
	const Vector& getPosition(const Entity& entity);
	Side::Enum getOrientation(const Component& component);
	Side::Enum getOrientation(const Entity& entity);
	const OBB& getShape(const Component& component);
	const OBB& getShape(const Entity& entity);
	bool raycast(Entity& entity, const Vector& rayDirection, int mask, RayCastResult& result);
	bool raycast(Entity& entity, const Vector& rayDirection, int mask, RayCastResult& result, const Vector& positionOffset);
	bool iterateTiles(Entity& entity, const Vector& radius, int mask, FixtureList* result);

	Hash getHashParam(void* data);
	const Vector& getVectorParam(void* data);
	inline float getFloatParam(void* data) { return *static_cast<float*>(data); }
	inline int getIntParam(void* data) { return *static_cast<int*>(data); }
	inline bool getBoolParam(void* data) { return *static_cast<bool*>(data); }

	void sendDirectionAction(const Component& component, Side::Enum direction);
	void handleGridFixtureMessage(Message& message, Component& component, Fixture& fixture);
	void handleNonGridFixtureMessage(Message& message, Component& component, Fixture& fixture);

	class SensorParams
	{
	public:
		SensorParams(Hash sensorId, const Contact* contact = 0)
			: _sensorId(sensorId), _contact(contact), _isHandled(false) {}

		Hash getSensorId() const { return _sensorId; }
		const Contact& getContact() const { return *_contact; }
		void setHandled(bool isHandled) { _isHandled = isHandled; }
		bool isHandled() const { return _isHandled; }
	private:
		Hash _sensorId;
		const Contact* _contact;
		bool _isHandled;

		SensorParams(const SensorParams&);
		SensorParams& operator=(const SensorParams&);
	};
	inline SensorParams& getSensorParams(void* data) { return *static_cast<SensorParams*>(data); }

	class MouseParams
	{
	public:
		MouseParams(MouseButton::Enum button, const Vector& position)
			: _button(button), _position(position), _sender(0), _handled(false) {}

		MouseButton::Enum getButton() const { return _button; }
		const Vector& getPosition() const { return _position; }
		void* getSender() const { return _sender; }
		void setSender(void* sender) { _sender = sender; }
		void setHandled(bool handled) { _handled = handled; }
		bool isHandled() const { return _handled; }

	private:
		MouseButton::Enum _button;
		const Vector& _position;
		void* _sender;
		bool _handled;

		MouseParams(const MouseParams&);
		MouseParams& operator=(const MouseParams&);
	};
	inline MouseParams& getMouseParams(void* data) { return *static_cast<MouseParams*>(data); }

	class AnimationParams
	{
	public:
		explicit AnimationParams(Hash animationId, bool rewind = false, float normalizedTime = 0.0f, int layer = 0, float weight = 1.0f) :
		_animationId(animationId), _rewind(rewind), _normalizedTime(normalizedTime), _layer(layer), _weight(weight) {}

		Hash getAnimationId() const { return _animationId; }
		bool isRewind() const { return _rewind; }
		float getNormalizedTime() const { return _normalizedTime; }
		int getLayer() const { return _layer; }
		float getWeight() const { return _weight; }

	private:
		Hash _animationId;
		bool _rewind;
		float _normalizedTime;
		int _layer;
		float _weight;
		
		AnimationParams(const AnimationParams&);
		AnimationParams& operator=(const AnimationParams&);
	};

	inline AnimationParams& getAnimationParams(void* data) { return *static_cast<AnimationParams*>(data); }
}
#endif