#include "BasePanel.h"
#include "DebugInfo.h"
#include "EntitySystem.h"
#include "Game.h"
#include "Shapes.h"
#include "NumericPair.h"
#include "ViewManager.h"
#include "Graphics.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include "Input.h"
#include "ActionController.h"
#include "TemporalPeriod.h"

namespace Temporal
{
	void BasePanel::init()
	{
		Size screenSize = Size(1024.0f, 768.0f);
		ViewManager::get().init(screenSize, 768.0f);
		Size worldSize = Size(screenSize.getWidth() * 2.0f, screenSize.getHeight());
		ViewManager::get().setLevelBounds(worldSize);
		Grid::get().init(worldSize, 128.0f);
		DebugInfo::get().setShowingFPS(true);

		createEntities();

		NavigationGraph::get().init();

		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_CREATED));
	}

	void BasePanel::update(float framePeriodInMillis)
	{
		Input::get().update();
		
		if(Input::get().isQuit())
		{
			Game::get().stop();
		}
		if(Input::get().isQ())
		{
			//AABB bounds = AABB::Zero;
			//EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_BOUNDS, &bounds));
			//EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, static_cast<void*>(&bounds));
			Period::Enum period = Period::Past;
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &period));
			//EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));

		}
		if(Input::get().isW())
		{
			Period::Enum period = Period::Present;
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &period));
		}
		if(Input::get().isE())
		{
			//Period::Enum period = Period::Future;
			//EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_PERIOD, &period));
		}
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));		
	}


	void BasePanel::draw() const
	{
		ViewManager::get().update();
		DebugInfo::get().draw();
		for(int i = VisualLayer::FARTHEST; i <= VisualLayer::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));

		ComponentType::Enum filter = ComponentType::STATIC_BODY;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DEBUG_DRAW), filter);
		//Grid::get().draw();
		//NavigationGraph::get().draw();
	}

	void BasePanel::dispose()
	{
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_DESTROYED));
		JumpInfoProvider::get().dispose();
		HashToString::get().dispose();
		EntitiesManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
		NavigationGraph::get().dispose();
	}
}

