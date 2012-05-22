#ifndef STATICBODY_H
#define STATICBODY_H

#include "Component.h"

namespace Temporal
{
	class Shape;

	class StaticBody : public Component
	{
	public:
		StaticBody(Shape* shape, bool cover) : _shape(shape), _cover(cover) {};
		~StaticBody(void);

		bool isCover(void) const { return _cover; }
		const Shape& getShape(void) const { return *_shape; }
		ComponentType::Enum getType(void) const { return ComponentType::STATIC_BODY; }
		void handleMessage(Message& message);

	private:
		bool _cover;

		// TODO: WTF why pointer?
		Shape* _shape;
	};
}
#endif