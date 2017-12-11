#ifndef LIGHT_EDITOR_H
#define LIGHT_EDITOR_H

#include "ComponentEditor.h"

namespace Temporal
{
	class Light;

	class LightEditor : public ComponentEditor
	{
	public:
		LightEditor(Light& light) :_light(light) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Light& _light;

		void activatedChanged(bool b);
	};
}

#endif