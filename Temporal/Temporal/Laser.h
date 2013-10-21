#ifndef LASER_H
#define LASER_H

#include "EntitySystem.h"
#include "Hash.h"

namespace Temporal
{
	class SceneNode;

	class Laser : public Component
	{
	public:
		explicit Laser() {}
		
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Laser(_platformID); };
		static const Hash TYPE;
	private:
		SceneNode* _root;

		void update(float framePeriod);

		friend class SerializationAccess;
	};
}

#endif