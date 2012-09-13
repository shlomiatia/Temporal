#include "TestLevel.h"
#include "Vector.h"
#include "Shapes.h"
#include "Math.h"
#include "EntitySystem.h"
#include "Transform.h"
#include "DrawPosition.h"
#include "ActionController.h"
#include "MovementUtils.h"
#include "TemporalEcho.h"
#include "TemporalPeriod.h"
#include "StaticBody.h"
#include "DynamicBody.h"
#include "Grid.h"
#include "Sensor.h"
#include "Sight.h"
#include "InputController.h"
#include "SpriteSheet.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Graphics.h"
#include "Animator.h"
#include "Animation.h"
#include "Renderer.h"
#include "Navigator.h"
#include "Sentry.h"
#include "Patrol.h"
#include "SecurityCamera.h"
#include "Laser.h"
#include "MessageUtils.h"
#include "Lighting.h"
#include "Particles.h"
#include "Fixture.h"
#include "CollisionFilter.h"
#include "PhysicsEnums.h"
#include "Input.h"
#include "Camera.h"
#include "Layer.h"
#include "Serialization.h"
#include "ResourceManager.h"
#include "tinyxml2.h"

namespace Temporal
{
	void TestLevel::init()
	{
		Size resolution = Size(1280.0f, 720.0f);
		Size levelSize = Size(2048.0f, 720.0f);
		float relativeHeight = 720.0f;

		Grid::get().init(levelSize, 128.0f);
		Graphics::get().init(resolution, relativeHeight);
		ResourceManager::get().init();

		LayersManager::get().add(new Camera(levelSize));
		LayersManager::get().add(new SpriteLayer());
		//LayersManager::get().add(new LightLayer(Color(0.1f, 0.1f, 0.1f)));
		LayersManager::get().add(new DebugLayer());

		EntitiesManager::get().init();

		NavigationGraph::get().init();
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::LEVEL_INIT));
	}

	void TestLevel::update(float framePeriod)
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
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}

		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));		
	}

	void TestLevel::draw() const
	{
		LayersManager::get().draw();
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
