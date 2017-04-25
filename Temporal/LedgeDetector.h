#ifndef LEDGEDETECTOR_H
#define LEDGEDETECTOR_H

#include "Hash.h"
#include "Sensor.h"

namespace Temporal
{
	class LedgeDetector : public ContactListener
	{
	public:
		explicit LedgeDetector(Hash sensorId, Component& owner)
			: ContactListener(sensorId, owner), _up(0), _upFailed(false), _down(0), _downFailed(false), _front(0), _frontFailed(false), _height(0.0f), _max(0.0f), _body(0), _side(Side::LEFT) {}

	protected:
		void start();
		void handle(const Contact& contact);
		void end();

	private:
		Fixture* _up;
		bool _upFailed;
		Fixture* _down;
		bool _downFailed;
		Fixture* _front;
		bool _frontFailed;
		float _height;
		float _max;
		OBBAABBWrapper _body;
		Side::Enum _side;

		void handleUp(const Contact& contact);
		void handleDown(const Contact& contact);
		void handleFront(const Contact& contact);

		void handleFrontCheckY(float y);
	};
}

#endif