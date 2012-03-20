#include "BasePanel.h"

#include "DebugInfo.h"
#include "Message.h"
#include "EntitiesManager.h"
#include "Game.h"
#include <Temporal\Base\Vector.h>
#include <Temporal\Graphics\ViewManager.h>
#include <Temporal\Graphics\Graphics.h>
#include <Temporal\Physics\Grid.h>
#include <Temporal\AI\NavigationGraph.h>
#include <Temporal\Input\Input.h>


namespace Temporal
{
	void BasePanel::init(void)
	{
		Vector screenSize = Vector(1024.0f, 768.0f);
		ViewManager::get().init(screenSize, 768.0f);
		Vector worldSize(screenSize);
		ViewManager::get().setLevelBounds(screenSize);
		Grid::get().init(worldSize, 32.0f);
		DebugInfo::get().setShowingFPS(true);

		createEntities();

		NavigationGraph::get().init();
	}

	void BasePanel::update(float framePeriodInMillis)
	{
		Input::get().update();
		
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));
		const Vector& position = *(Vector*)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));
		
		ViewManager::get().update();
		
		if(Input::get().isQuit())
		{
			//EntitiesManager::get().sendMessageToEntity(1, Message(MessageID::SET_NAVIGATION_DESTINATION, &position));
			Game::get().stop();
		}
	}

	void BasePanel::draw(void) const
	{
		DebugInfo::get().draw();
		for(int i = VisualLayer::FARTHEST; i <= VisualLayer::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));

		// TODO: Classify debug draw SLOTH
		bool debugDraw = false;
		if(debugDraw)
		{
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DEBUG_DRAW));
			//Grid::get().draw();
			//NavigationGraph::get().draw();
		}
	}

	void BasePanel::dispose(void)
	{
		EntitiesManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
		NavigationGraph::get().dispose();
	}
}

