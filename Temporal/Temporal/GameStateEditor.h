#ifndef GAME_STATE_EDITOR_H
#define GAME_STATE_EDITOR_H

#include "ToolComponent.h"

namespace Temporal
{
	class Vector;
	class MouseParams;

	class GameStateEditor : public ToolComponent
	{
	public:
		GameStateEditor() {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static const Hash TYPE;
	private:
		void setEditorMode();
		void cloneEntityFromTemplate(Hash id, Vector& position, bool bypassSave = false);
		void clearCursor();
		void update(float framePeriod);
		void handleKey(Key::Enum key);
		void moveCamera(const Vector& direction);
		void leftClick(const MouseParams& params);
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