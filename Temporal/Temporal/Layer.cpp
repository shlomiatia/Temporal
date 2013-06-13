#include "Layer.h"
#include "EntitySystem.h"
#include "Graphics.h"
#include "Thread.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include "Hash.h"
#include "Timer.h"
#include "Log.h"
#include "Camera.h"
#include <SDL_opengl.h>

namespace Temporal
{
	static const Hash FPS_TIMER = Hash("TMR_FPS");

	void LayersManager::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);

		// TODO: Settings
		Size levelSize = Size(3840.0f, 720.0f);
		
		_layers.push_back(new Camera(levelSize));
		_layers.push_back(new SpriteLayer());
		//_layers.push_back(new LightLayer(Color(0.1f, 0.1f, 0.1f)));
		_layers.push_back(new DebugLayer());
		_layers.push_back(new GUILayer());

		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			(**i).init(this);
		}
	}

	LayersManager::~LayersManager()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			delete *i;
		}
	}

	void LayersManager::draw()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			(**i).draw();
	}

	void SpriteLayer::draw()
	{
		
		for(int i = LayerType::FARTHEST; i <= LayerType::NEAREST; ++i)
			_manager->getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW, &i));
	}

	void GUILayer::draw()
	{
		glLoadIdentity();
		LayerType::Enum guiLayer = LayerType::GUI;
		_manager->getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW, &guiLayer));
	}

	void DebugLayer::drawFPS()
	{
		PerformanceTimer& timer = PerformanceTimerManager::get().getTimer(FPS_TIMER);
		
		if(!timer.isStarted())
		{
			timer.start();
		}
		else
		{
			timer.split();
			float time = timer.getElapsedTime();
			if(time >= 1.0f)
			{
				timer.stop();
				float fps = timer.getSplits() / time;
				Log::write("FPS: %f\n", fps);
				timer.start();
			}
		}
	}

	void DebugLayer::draw()
	{
		ComponentType::Enum filter = ComponentType::STATIC_BODY;
		_manager->getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), filter);
		
		//Grid::get().draw();
		//NavigationGraph::get().draw();

		drawFPS();
	}
}