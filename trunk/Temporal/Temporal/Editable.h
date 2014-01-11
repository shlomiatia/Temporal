#ifndef EDITABLE_H
#define EDITABLE_H
#include "EntitySystem.h"
#include "Delegate.h"
#include "Shapes.h"

namespace Temporal
{
	class MouseParams;

	class Editable : public Component
	{
	public:
		explicit Editable() : _translation(false), _rotation(false), _offset(Vector::Zero) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Editable(); }

		static const Hash TYPE;
	private:
		bool _translation;
		bool _rotation;
		Vector _offset;

		void leftMouseDown(const MouseParams& params);
		void rightMouseDown(const MouseParams& params);
		void mouseMove(const MouseParams& params);
	};
}
#endif