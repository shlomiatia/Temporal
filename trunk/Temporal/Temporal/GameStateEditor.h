#ifndef GAME_STATE_EDITOR_H
#define GAME_STATE_EDITOR_H

#include "ToolComponent.h"
#include "Timer.h"

namespace Temporal
{
	class Vector;
	class MouseParams;
	class Editable;

	class GameStateEditor : public ToolComponent
	{
	public:
		GameStateEditor() : _selectedEditable(0) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		void setSelected(Editable* selected) { _selectedEditable = selected; }
		Editable* getSelected() { return _selectedEditable; }

		static const Hash TYPE;
	private:
		Timer _autoSaveTimer;
		Editable* _selectedEditable;

		void setEditorMode();
		void addEntity(Entity* newEntity, Hash id, bool bypassSave = false);
		void clearCursor();
		void update(float framePeriod);
		void handleKey(Key::Enum key);
		void moveCamera(const Vector& direction);
		void leftClick(const MouseParams& params);
		void save();
	};

	class GameStateEditorPreview : public Component
	{
	public:
		GameStateEditorPreview() {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static const Hash TYPE;
	private:
	};
}
#endif