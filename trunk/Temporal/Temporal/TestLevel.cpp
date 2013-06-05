#include "TestLevel.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "EntitySystem.h"
#include "Grid.h"
#include "Graphics.h"
#include "PhysicsEnums.h"
#include "Input.h"
#include "Camera.h"
#include "Layer.h"
#include "Navigator.h"
#include "ResourceManager.h"
#include "Lighting.h"
#include "tinyxml2.h"
#include <ftgl/ftgl.h>
namespace Temporal
{
	FTGLPixmapFont font("c:/windows/fonts/Arial.ttf");
	
	void TestLevel::init()
	{
		font.FaceSize(72);
		Size resolution = Size(1280.0f, 720.0f);
		Size levelSize = Size(3840.0f, 720.0f);
		float relativeHeight = 720.0f;

		Grid::get().init(levelSize, 128.0f);
		Graphics::get().init(resolution, relativeHeight);
		ResourceManager::get().init();

		LayersManager::get().add(new Camera(levelSize));
		LayersManager::get().add(new SpriteLayer());
		//LayersManager::get().add(new LightLayer(Color.1f, 0.1f, 0.1f)));
		LayersManager::get().add(new DebugLayer());

		EntitiesManager::get().init();

		NavigationGraph::get().init();
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_INIT));
	}

	void TestLevel::update(float framePeriod)
	{
		Input::get().update();
		
		if(Input::get().key(Key::ESC))
		{
			Game::get().stop();
		}
		if(Input::get().key(Key::Q))
		{
			const AABB& bounds = *static_cast<AABB*>(EntitiesManager::get().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			EntitiesManager::get().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			//EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}

		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));		
	}

	void TestLevel::draw() const
	{
		LayersManager::get().draw();
		//font.Render("Hello World!");
	}

	void TestLevel::dispose()
	{
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_DISPOSED));
		NavigationGraph::get().dispose();
		EntitiesManager::get().dispose();
		LayersManager::get().dispose();
		ResourceManager::get().dispose();
		Graphics::get().dispose();
		Grid::get().dispose();
		HashToString::get().dispose();
	}
}
