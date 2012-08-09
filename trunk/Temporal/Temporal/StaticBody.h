#ifndef STATICBODY_H
#define STATICBODY_H

#include "EntitySystem.h"

namespace Temporal
{
	class CollisionInfo;

	class StaticBody : public Component
	{
	public:
		StaticBody(CollisionInfo* collisionInfo) : _collisionInfo(collisionInfo) {};
		~StaticBody();

		CollisionInfo& getCollisionInfo() { return *_collisionInfo; }

		ComponentType::Enum getType() const { return ComponentType::STATIC_BODY; }
		void handleMessage(Message& message);

	private:
		CollisionInfo* _collisionInfo;
	};
}
#endif