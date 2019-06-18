#pragma once

#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class Beacon : public Component
	{
	public:
		Hash getType() const { return ComponentsIds::BEACON; }
		void handleMessage(Message& message);
		Component* clone() const { return new Beacon(); };

	private:
		friend class SerializationAccess;
	};
}