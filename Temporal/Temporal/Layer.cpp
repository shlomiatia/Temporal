#include "Layer.h"
#include "EntitySystem.h"
#include "Graphics.h"
#include "Thread.h"
#include "Grid.h"
//#include "NavigationGraph.h"
#include "Hash.h"
#include "Timer.h"
#include "Log.h"
#include "Camera.h"
//#include "Lighting.h"
#include "StaticBody.h"
#include "Sensor.h"
#include "DynamicBody.h"
#include <SDL_opengl.h>

namespace Temporal
{
	static const Hash FPS_TIMER = Hash("TMR_FPS");

	void LayersManager::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);

		_camera = new Camera(this, _cameraFollowPlayer);
		_spriteLayer = new SpriteLayer(this);
		_guiLayer = new GUILayer(this);
		
			
		_layers.push_back(_camera);
		_layers.push_back(_spriteLayer);
/*		if(_ambientColor != Color::White)
			_layers.push_back(new LightLayer(this, _ambientColor));*/
		_layers.push_back(new DebugLayer(this));
		_layers.push_back(_guiLayer);
	}

	LayersManager::~LayersManager()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			delete *i;
	}

	void LayersManager::draw()
	{		
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			(**i).draw();
		}
	}

	SpriteLayer::SpriteLayer(LayersManager* manager) : Layer(manager)
	{
		for(int i = 0; i <= LayerType::SIZE; ++i)
			_layers[static_cast<LayerType::Enum>(i)] = ComponentCollection();
	}

	void SpriteLayer::draw()
	{
		for(int i = 0; i < LayerType::SIZE; ++i)
		{
			ComponentCollection& components = _layers.at(static_cast<LayerType::Enum>(i));
			for(ComponentIterator j = components.begin(); j != components.end(); ++j)
			{
				(**j).handleMessage(Message(MessageID::DRAW));
			}
		}
	}

	void GUILayer::draw()
	{
		//glLoadIdentity();
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW));
		}
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
				Log::write("FPS: %f", fps);
				timer.start();
			}
		}
	}

	void DebugLayer::draw()
	{
		HashCollection filter;
		filter.push_back(StaticBody::TYPE);
		filter.push_back(DynamicBody::TYPE);
		getManager().getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), &filter);
		
		//Grid::get().draw();
		//NavigationGraph::get().draw();

		drawFPS();
	}
}