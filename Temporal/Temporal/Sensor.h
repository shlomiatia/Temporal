#ifndef SENSOR_H
#define SENSOR_H

#include "Hash.h"
#include "BaseEnums.h"
#include "Vector.h"
#include "EntitySystem.h"
#include "Fixture.h"

namespace Temporal
{
	class Contact
	{
	public:
		Contact(const Fixture& source, const Fixture& target) : _source(source), _target(target) {}

		const Fixture& getSource() const { return _source; }
		const Fixture& getTarget() const { return _target; }
	private:
		const Fixture& _source;
		const Fixture& _target;

		Contact(const Contact&);
		Contact& operator=(const Contact&);
	};

	class ContactListener
	{
	public:
		ContactListener() : _owner(NULL) {};
		virtual ~ContactListener() {};
	
		void setOwner(const Component& owner) { _owner = &owner; }

		virtual void start() {}
		virtual void handle(const Contact& contact) = 0;
		virtual void end() {}
		virtual ContactListener* clone() const = 0;

	protected:
		const Component& getOwner() { return *_owner; }
	private:
		const Component* _owner;

		ContactListener(const ContactListener&);
		ContactListener& operator=(const ContactListener&);
	};

	class Sensor : public Component
	{
	public:
		explicit Sensor(Fixture* fixture = NULL, ContactListener* listener = NULL, int categoryMask = -1)
			: _fixture(fixture), _listener(listener), _categoryMask(categoryMask) {}
		~Sensor() { delete _fixture; delete _listener; }
		
		void handleMessage(Message& message);
		ComponentType::Enum getType() const { return ComponentType::SENSOR; }

		Component* clone() const;
	private:
		int _categoryMask;
		Fixture* _fixture;
		ContactListener* _listener;

		void update();

		friend class SerializationAccess;
	};

	class LedgeDetector : public ContactListener
	{
	public:
		explicit LedgeDetector(Hash id = Hash::INVALID, float rangeCenter = 0.0f, float rangeSize = 0.0f)
			: _id(id), _point(Vector::Zero), _rangeCenter(rangeCenter), _rangeSize(rangeSize), _isBlocked(false) {}

		void start();
		void handle(const Contact& contact);
		void end();

		ContactListener* clone() const { return new LedgeDetector(_id, _rangeCenter, _rangeSize); }
	private:
		Hash _id;
		float _rangeCenter;
		float _rangeSize;
		Vector _point;
		bool _isBlocked;

		friend class SerializationAccess;
	};
}

#endif