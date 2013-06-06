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
		ContactListener(Hash sensorId) : _sensorId(sensorId) {};
		virtual ~ContactListener() {};
	
		void handleMessage(Message& message);

	protected:
		virtual void start() {}
		virtual void handle(const Contact& contact) = 0;
		virtual void end() {}

	private:
		Hash _sensorId;

		ContactListener(const ContactListener&);
		ContactListener& operator=(const ContactListener&);
	};

	class Sensor : public Component
	{
	public:
		explicit Sensor(const Hash& id = Hash::INVALID, Fixture* fixture = 0, int categoryMask = -1)
			: _id(id), _fixture(fixture), _categoryMask(categoryMask) {}
		~Sensor() { delete _fixture; }
		
		void handleMessage(Message& message);
		ComponentType::Enum getType() const { return ComponentType::SENSOR; }
		Component* clone() const;

		const Hash& getId() const { return _id; }

	private:
		Hash _id;
		int _categoryMask;
		Fixture* _fixture;
		ContactListener* _listener;

		void update();

		friend class SerializationAccess;
	};
}

#endif