#ifndef STATICBODY_H
#define STATICBODY_H

#include "EntitySystem.h"

namespace Temporal
{
	class Shape;

	class StaticBody : public Component
	{
	public:
		StaticBody(Shape* shape, bool cover) : _shape(shape), _cover(cover) {};
		~StaticBody();

		bool isCover() const { return _cover; }
		const Shape& getShape() const { return *_shape; }
		ComponentType::Enum getType() const { return ComponentType::STATIC_BODY; }
		void handleMessage(Message& message);
		int getCollisionFilter() const;

	private:
		bool _cover;
		Shape* _shape;
	};
}
#endif