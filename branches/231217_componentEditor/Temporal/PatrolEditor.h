#ifndef PATROL_EDITOR_H
#define PATROL_EDITOR_H

#include "ComponentEditor.h"

namespace Temporal
{
	class Patrol;

	class PatrolEditor : public ComponentEditor
	{
	public:
		PatrolEditor(Patrol& patrol) : _patrol(patrol) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Patrol& _patrol;

		void isStaticChanged(bool b);
		void securityCameraIdChanged(const char* s);
	};
}

#endif

