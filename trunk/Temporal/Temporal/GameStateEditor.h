#ifndef GAME_STATE_EDITOR_H
#define GAME_STATE_EDITOR_H

#include "EntitySystem.h"

namespace Temporal
{
	class Vector;

	class GameStateEditor : public Component
	{
	public:
		GameStateEditor() {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		void moveCamera(const Vector& direction);
	};

	class GSEGameStateListener : public GameStateListener
	{
	public:
		void onLoaded(Hash id, GameState& gameState);
	};
}
#endif