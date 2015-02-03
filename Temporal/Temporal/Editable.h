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
		explicit Editable() : _translation(false), _rotation(false), _scale(false), _isPositiveScale(false), _scaleAxis(Axis::X), _translationOffset(Vector::Zero) {}

		Hash getType() const;
		void handleMessage(Message& message);
		Component* clone() const { return new Editable(); }
	private:
		bool _translation;
		bool _rotation;
		bool _scale;
		bool _isPositiveScale;
		Axis::Enum _scaleAxis;
		Vector _translationOffset;
		OBB _positiveXScale;
		OBB _negativeXScale;
		OBB _positiveYScale;
		OBB _negativeYScale;

		void mouseMove(const MouseParams& params);
		void mouseDown(const MouseParams& params);
		void reset();
	};
}
#endif