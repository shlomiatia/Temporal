#ifndef EDITABLE_H
#define EDITABLE_H
#include "EntitySystem.h"
#include "Shapes.h"

namespace Temporal
{
	class MouseParams;
	class GameStateEditor;

	class Editable : public Component
	{
	public:
		explicit Editable(bool translationOnly, GameStateEditor& editor) : Component(true),
			_translationOnly(translationOnly), _translation(false), _rotation(false), _scale(false), _isPositiveScale(false), _scaleAxis(Axis::X), _translationOffset(Vector::Zero), _editor(editor){}

		Hash getType() const;
		void handleMessage(Message& message);
		Component* clone() const { return new Editable(_translationOnly, _editor); }

		static OBB getShape(const Entity& entity);

		static const Hash TYPE;
		
	private:
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
		GameStateEditor& _editor;

		void mouseMove(MouseParams& params);
		void leftMouseDown(MouseParams& params);
		void middleMouseDown(MouseParams& params);
		void rightMouseDown(MouseParams& params);
		void mouseDown(MouseParams& params);
		void reset();
		void handleArrows(const Vector& params);

		OBB getShape() const;
		void setRotation(float rotation);
		void setRadius(const Vector& radius);
		void setPeriod(int period);
	};
}
#endif