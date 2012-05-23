#include "BasePanel.h"

#include "DebugInfo.h"
#include "Message.h"
#include "EntitiesManager.h"
#include "Game.h"
#include "AABB.h"
#include "NumericPair.h"
#include "ViewManager.h"
#include "Graphics.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include "Input.h"
#include "ActionController.h"

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
			AABB bounds = AABB::Zero;
			EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_BOUNDS, &bounds));
			EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, (void*)&bounds));
		}
		if(Input::get().isW())
		{
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriodInMillis));
		
		ViewManager::get().update();
	}

	void BasePanel::draw(void) const
	{
		DebugInfo::get().draw();
		for(int i = VisualLayer::FARTHEST; i <= VisualLayer::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));

		ComponentType::Enum filter = ComponentType::STATIC_BODY | ComponentType::SIGHT | ComponentType::DYNAMIC_BODY;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DEBUG_DRAW), filter);
		//Grid::get().draw();
		//NavigationGraph::get().draw();
	}

	void BasePanel::dispose(void)
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

