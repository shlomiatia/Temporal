#include "BasePanel.h"

#include "DebugInfo.h"
#include "Message.h"
#include "EntitiesManager.h"
#include "Game.h"
#include <Temporal\Base\AABB.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Graphics\ViewManager.h>
#include <Temporal\Graphics\Graphics.h>
#include <Temporal\Physics\Grid.h>
#include <Temporal\AI\NavigationGraph.h>
#include <Temporal\Input\Input.h>

namespace Temporal
{
	void BasePanel::init(void)
	{
		Size screenSize = Size(1024.0f, 768.0f);
		ViewManager::get().init(screenSize, 768.0f);
		Size worldSize = Size(screenSize.getWidth() * 2.0f, screenSize.getHeight());
		ViewManager::get().setLevelBounds(worldSize);
		Grid::get().init(worldSize, 128.0f);
		DebugInfo::get().setShowingFPS(true);

		createEntities();

		NavigationGraph::get().init();
	}

	void BasePanel::update(float framePeriodInMillis)
	{
		Input::get().update();
		
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));
		
		ViewManager::get().update();
		
		if(Input::get().isQuit())
		{
			AABB bounds = AABB::Empty;
			EntitiesManager::get().sendMessageToEntity(Hash("PLAYER"), Message(MessageID::GET_BOUNDS, &bounds));
			EntitiesManager::get().sendMessageToEntity(Hash("CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, (void*)&bounds));
			//Game::get().stop();
		}
	}

	void BasePanel::draw(void) const
	{
		DebugInfo::get().draw();
		for(int i = VisualLayer::FARTHEST; i <= VisualLayer::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));

		ComponentType::Enum filter = ComponentType::STATIC_BODY | ComponentType::AI_CONTROLLER;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DEBUG_DRAW), filter);
		//Grid::get().draw();
		//NavigationGraph::get().draw();
	}

	void BasePanel::dispose(void)
	{
		EntitiesManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
		NavigationGraph::get().dispose();
	}
}

