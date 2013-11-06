#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include "BaseEnums.h"
#include "Hash.h"

namespace Temporal
{
	class Component;
	class Message;
	class YABP;
	class Vector;
	class Contact;
	namespace MouseButton { enum Enum; }

	const Vector& getPosition(const Component& component);
	Side::Enum getOrientation(const Component& component);

	Hash getHashParam(void* data);
	const Vector& getVectorParam(void* data);
	inline float getFloatParam(void* data) { return *static_cast<float*>(data); }
	inline int getIntParam(void* data) { return *static_cast<int*>(data); }
	inline bool getBoolParam(void* data) { return *static_cast<bool*>(data); }

	void sendDirectionAction(const Component& component, Side::Enum direction);

	class SensorParams
	{
	public:
		SensorParams(Hash sensorId, const Contact* contact = 0)
			: _sensorId(sensorId), _contact(contact) {}

		Hash getSensorId() const { return _sensorId; }
		const Contact& getContact() const { return *_contact; }
	private:
		Hash _sensorId;
		const Contact* _contact;

		SensorParams(const SensorParams&);
		SensorParams& operator=(const SensorParams&);
	};
	inline const SensorParams& getSensorParams(void* data) { return *static_cast<SensorParams*>(data); }

	class MouseParams
	{
	public:
		MouseParams(MouseButton::Enum button, const Vector& position)
			: _button(button), _position(position), _sender(0)/*, _isHandled(false)*/ {}

		MouseButton::Enum getButton() const { return _button; }
		const Vector& getPosition() const { return _position; }
		void* getSender() const { return _sender; }
		void setSender(void* sender) { _sender = sender; }
//		bool isHandled() const { return _isHandled; }
//		void setHandled(bool isHandled) { _isHandled = isHandled; }

	private:
		MouseButton::Enum _button;
		const Vector& _position;
		void* _sender;
		//bool _isHandled;

		MouseParams(const MouseParams&);
		MouseParams& operator=(const MouseParams&);
	};
	inline MouseParams& getMouseParams(void* data) { return *static_cast<MouseParams*>(data); }
}
#endif