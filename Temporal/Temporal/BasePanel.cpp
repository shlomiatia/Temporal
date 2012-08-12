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
#include "Lighting.h"

namespace Temporal
{
	void BasePanel::init()
	{
		Size screenSize = Size(1024.0f, 768.0f);
		ViewManager::get().init(screenSize, 768.0f);
		LightSystem::get().init(screenSize);
		Size worldSize = Size(screenSize.getWidth() * 2.0f, screenSize.getHeight());
		ViewManager::get().setLevelBounds(worldSize);
		Grid::get().init(worldSize, 128.0f);
		DebugInfo::get().setShowingFPS(true);

		createEntities();

		NavigationGraph::get().init();
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_CREATED));
		Period::Enum period = Period::PRESENT;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_CURRENT_PERIOD, &period));
	}

	void ChangePeriod(Period::Enum period)
	{
		EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::SET_PERIOD, &period));
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::SET_CURRENT_PERIOD, &period));
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
			//const AABB& bounds = *static_cast<AABB*>(EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			//EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			//ChangePeriod(Period::PAST);
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));

		}
		if(Input::get().isW())
		{
			ChangePeriod(Period::PRESENT);
		}
		if(Input::get().isE())
		{
			ChangePeriod(Period::FUTURE);
		}
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));		
	}

	void BasePanel::draw() const
	{
		ViewManager::get().update();
		DebugInfo::get().draw();

		for(int i = VisualLayer::FARTHEST; i <= VisualLayer::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));

		/*LightSystem::get().preDraw();
		VisualLayer::Enum lightLayer = VisualLayer::LIGHT;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &lightLayer));
		LightSystem::get().postDraw();*/

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

