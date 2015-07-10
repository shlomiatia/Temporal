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
		explicit Editable(bool translationOnly) : Component(true),
			_translationOnly(translationOnly), _translation(false), _rotation(false), _scale(false), _isPositiveScale(false), _scaleAxis(Axis::X), _translationOffset(Vector::Zero) {}

		Hash getType() const;
		void handleMessage(Message& message);
		Component* clone() const { return new Editable(_translationOnly); }

		static const Hash TYPE;
		static void clearSelected() { _selected = 0; }
		static Editable* getSelected() { return _selected;  }
	private:
		static Editable* _selected;
		bool _translationOnly;
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

		void mouseMove(MouseParams& params);
		void leftMouseDown(MouseParams& params);
		void rightMouseDown(MouseParams& params);
		void mouseDown(MouseParams& params);
		void reset();

		OBB getShape() const;
		void setRotation(float rotation);
		void setRadius(const Vector& radius);
	};
}
#endif