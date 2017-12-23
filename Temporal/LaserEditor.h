#ifndef LASER_EDITOR_H
#define LASER_EDITOR_H

#include "ComponentEditor.h"
#include "Laser.h"

namespace Temporal
{
	class Laser;
	
	class LaserEditor : public ComponentEditor
	{
	public:
		LaserEditor(Laser& laser) : _laser(laser) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Laser& _laser;

		void friendlyChanged(bool b);
		void speedPerSecondChanged(const char* s);
	};
}

#endif