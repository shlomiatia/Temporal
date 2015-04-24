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

		void setEditorMode(bool editorMode);

		static const Hash TYPE;
	private:
		HashEntityMap _templates;
		HashEntityIterator _templateIterator;

		void cloneEntityFromTemplate(Hash id, Vector& position);

		void moveCamera(const Vector& direction);
		void leftClick(const MouseParams& params);
	};

	class GSEGameStateListener : public GameStateListener
	{
	public:
		void onLoaded(Hash id, GameState& gameState);
	};
}
#endif