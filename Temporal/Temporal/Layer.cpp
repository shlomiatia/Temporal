#include "Layer.h"
#include "EntitySystem.h"
#include "Graphics.h"
#include "Thread.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include "Hash.h"
#include "Timer.h"
#include "Log.h"
#include <SDL_opengl.h>

namespace Temporal
{
	static const Hash FPS_TIMER = Hash("TMR_FPS");

	void LayersManager::draw()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			(**i).draw();
		
		glLoadIdentity();
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			(**i).drawGUI();
	}

	void LayersManager::dispose()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			delete *i;
		}
	}

	void SpriteLayer::draw()
	{
		for(int i = LayerType::FARTHEST; i <= LayerType::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));
	}

	void SpriteLayer::drawGUI()
	{
		LayerType::Enum guiLayer = LayerType::GUI;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &guiLayer));
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
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), filter);
		
		//Grid::get().draw();
		//NavigationGraph::get().draw();

		drawFPS();
	}
}