#ifndef GAME_STATE_EDITOR_H
#define GAME_STATE_EDITOR_H

#include "ToolComponent.h"
#include "Timer.h"
#include "Ids.h"

namespace Temporal
{
	class Vector;
	class MouseParams;
	class Editable;

	class GameStateEditor : public ToolComponent
	{
	public:
		GameStateEditor(int undo = 0) : _selectedEditable(0), _undo(undo), _idPrefix(0){}

		Hash getType() const { return ComponentsIds::GAME_STATE_EDITOR; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		void setSelected(Editable* selected) { _selectedEditable = selected; }
		Editable* getSelected() { return _selectedEditable; }
		void addUndo();

		static void loadEditor(const char* path = 0, int undo = 0);

		
	private:
		Timer _autoSaveTimer;
		Editable* _selectedEditable;
		int _undo;
		const char* _idPrefix;

		void setEditorMode();
		void addEntity(Entity* newEntity, Hash id, bool bypassSave = false);
		void deleteEntity();
		void clearCursor();
		void setCursor();
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

		Hash getType() const { return ComponentsIds::GAME_STATE_EDITOR_PREVIEW; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		
	private:
	};
}
#endif