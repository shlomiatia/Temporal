#ifndef DRAWPOSITION_H
#define DRAWPOSITION_H

#include "EntitySystem.h"
#include "Vector.h"

namespace Temporal
{
	class DrawPosition : public Component
	{
	public:
		explicit DrawPosition(const Vector& offset = Vector::Zero) : _offset(offset), _override(Vector::Zero) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		
		Component* clone() const { return new DrawPosition(_offset); }

		static const Hash TYPE;
	private:

		Vector _offset;
		Vector _override;

		friend class SerializationAccess;
	};
}
#endif