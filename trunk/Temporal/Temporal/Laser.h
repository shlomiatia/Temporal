#ifndef LASER_H
#define LASER_H

#include "EntitySystem.h"
#include "Hash.h"

namespace Temporal
{
	static const float LASER_SPEED_PER_SECOND = 50.0f;

	class SceneNode;

	class Laser : public Component
	{
	public:
		explicit Laser(Hash platformID = Hash::INVALID) 
			: _platformID(platformID), _isPositiveDirection(true) {}

		Hash getPlatformId() const { return _platformID; }
		
		ComponentType::Enum getType() const { return ComponentType::AI_CONTROLLER; }
		void handleMessage(Message& message);

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("platform", _platformID);
		}
	private:
		Hash _platformID;
		bool _isPositiveDirection;
		SceneNode* _root;

		void update(float framePeriod);
	};
}

#endif