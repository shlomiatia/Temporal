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

		const CollisionInfo& getCollisionInfo() const { return *_collisionInfo; }

		ComponentType::Enum getType() const { return ComponentType::STATIC_BODY; }
		void handleMessage(Message& message);
		int getCollisionFilter() const;

	private:
		const CollisionInfo* _collisionInfo;
	};
}
#endif