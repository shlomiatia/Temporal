#ifndef SENSOR_H
#define SENSOR_H

#include "Hash.h"
#include "BaseEnums.h"
#include "NumericPair.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Fixture;

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
		Sensor(Fixture* fixture, ContactListener* listener)
			: _fixture(fixture), _listener(listener) { _listener->setOwner(*this); }
		~Sensor() { delete _fixture; delete _listener; }
		
		void handleMessage(Message& message);
		ComponentType::Enum getType() const { return ComponentType::SENSOR; }

	private:
		Fixture* _fixture;
		ContactListener* _listener;

		void update();
	};

	class LedgeDetector : public ContactListener
	{
	public:
		LedgeDetector(const Hash& id, float rangeCenter, float rangeSize)
			: _id(id), _point(Point::Zero), _rangeCenter(rangeCenter), _rangeSize(rangeSize), _isBlocked(false) {}

		void start();
		void handle(const Contact& contact);
		void end();

	private:
		const Hash _id;
		const float _rangeCenter;
		const float _rangeSize;
		Point _point;
		bool _isBlocked;
	};
}

#endif