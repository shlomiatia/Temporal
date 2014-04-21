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

	PerformanceTimer& spriteLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_SPRITE_LAYER"));
	void SpriteLayer::draw()
	{
		spriteLayerTimer.measure();
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getSpriteBatch().begin();
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), 0);
		for(int i = 0; i < LayerType::SIZE; ++i)
		{
			ComponentCollection& components = _layers.at(static_cast<LayerType::Enum>(i));
			for(ComponentIterator j = components.begin(); j != components.end(); ++j)
			{
				(**j).handleMessage(Message(MessageID::DRAW));
			}
		}
		spriteLayerTimer.print("SPRITE");
	}

	PerformanceTimer& guiLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_GUI_LAYER"));
	void GUILayer::draw()
	{
		guiLayerTimer.measure();
		Graphics::get().getMatrixStack().reset();
		//Graphics::get().getSpriteBatch().end();
		//Graphics::get().getSpriteBatch().begin();
		//Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), -1);
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW));
		}
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW_BORDERS));
		}
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getSpriteBatch().begin();
		Graphics::get().getLinesSpriteBatch().end();
		Graphics::get().getLinesSpriteBatch().begin();
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), 1);
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW_TEXT));
		}
		guiLayerTimer.print("GUI");
	}

	void DebugLayer::drawFPS()
	{
		PerformanceTimer& timer = PerformanceTimerManager::get().getTimer(FPS_TIMER);
		timer.print("FPS");
		timer.measure();
	}

	PerformanceTimer& debugLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_DEBUG_LAYER"));
	void DebugLayer::draw()
	{
		debugLayerTimer.measure();
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getSpriteBatch().begin();
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), -1);
		HashCollection filter;
		filter.push_back(StaticBody::TYPE);
		getManager().getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), &filter);
		
		//Grid::get().draw();
		//NavigationGraph::get().draw();

		drawFPS();
		debugLayerTimer.print("DEBUG LAYER");
	}
}