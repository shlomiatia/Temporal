#include "Layer.h"
#include "EntitySystem.h"
#include "Graphics.h"
#include "Thread.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include "Hash.h"
#include "Timer.h"
#include "Camera.h"
#include "Lighting.h"
#include "Keyboard.h"
#include <SDL_opengl.h>
#include <algorithm>

namespace Temporal
{
	static const Hash FPS_TIMER = Hash("TMR_FPS");

	void LayersManager::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);

		_camera = new Camera(this, _cameraFollowPlayer);
		_spriteLayer = new SpriteLayer(this);
		_guiLayer = new GUILayer(this);
		_fxLayer = new FXLayer(this);
		_debugLayer = new DebugLayer(this);
			
		_layers.push_back(_camera);
		_layers.push_back(_spriteLayer);
		if(_ambientColor != Color::White)
		{
			_lightLayer = new LightLayer(this, _ambientColor);
			_layers.push_back(_lightLayer);
		}
		_layers.push_back(_debugLayer);
		_layers.push_back(_guiLayer);
	}


	LayersManager::~LayersManager()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			delete *i;
	}

	void LayersManager::draw(float framePeriod)
	{		
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			(**i).draw(framePeriod);
		}
	}

	void LayersManager::addLight(Component* component)
	{
		if(_lightLayer)
			_lightLayer->add(component);
	}

	SpriteLayer::SpriteLayer(LayersManager* manager) : Layer(manager)
	{
		for(int i = 0; i <= LayerType::SIZE; ++i)
			_layers[static_cast<LayerType::Enum>(i)] = ComponentList();
	}

	void SpriteLayer::innerDraw()
	{
		Graphics::get().getSpriteBatch().begin();
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), 0);
		for(int i = 0; i < LayerType::SIZE; ++i)
		{
			ComponentList& components = _layers.at(static_cast<LayerType::Enum>(i));
			for(ComponentIterator j = components.begin(); j != components.end(); ++j)
			{
				(**j).handleMessage(Message(MessageID::DRAW));
			}
		}
		Graphics::get().getSpriteBatch().end();
	}

	PerformanceTimer& spriteLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_SPRITE_LAYER"));
	void SpriteLayer::draw(float framePeriod)
	{
		spriteLayerTimer.measure();
		//getManager().getFXLayer().preDraw();
		innerDraw();
		/*getManager().getFXLayer().draw();
		innerDraw();
		getManager().getFXLayer().draw2();*/
		spriteLayerTimer.print("SPRITE LAYER");
	}

	void SpriteLayer::remove(LayerType::Enum layer, Component* component) 
	{ 
		std::vector<Component*>& components = _layers.at(layer);
		components.erase(std::remove(components.begin(), components.end(), component));
	}

	PerformanceTimer& guiLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_GUI_LAYER"));
	void GUILayer::draw(float framePeriod)
	{
		guiLayerTimer.measure();
		Graphics::get().getMatrixStack().top().reset();
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), -1);
		Graphics::get().getSpriteBatch().begin();
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW));
		}
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getLinesSpriteBatch().begin();
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW_BORDERS));
		}
		Graphics::get().getLinesSpriteBatch().end();
		Graphics::get().getSpriteBatch().begin();
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), 1);
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW_TEXT));
		}
		Graphics::get().getSpriteBatch().end();
		guiLayerTimer.print("GUI LAYER");
	}

	void DebugLayer::drawFPS()
	{
		PerformanceTimer& timer = PerformanceTimerManager::get().getTimer(FPS_TIMER);
		timer.print("=== FPS ===");
		timer.measure();
	}

	PerformanceTimer& debugLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_DEBUG_LAYER"));

	static const Hash STATIC_BODY_ID("static-body");
	static const Hash EDITABLE_ID("editable");
	static const Hash SIGHT_ID("sight");
	static const Hash DYNAMIC_BODY_ID("dynamic-body");
	static const Hash SENSOR_ID("sensor");

	void DebugLayer::draw(float framePeriod)
	{
		debugLayerTimer.measure();
		Graphics::get().getLinesSpriteBatch().begin();
		Graphics::get().getSpriteBatch().begin();

		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), -1);
		HashList filter;
		filter.push_back(STATIC_BODY_ID);
		filter.push_back(EDITABLE_ID);
		if (_sight)
			filter.push_back(SIGHT_ID);
		if (_dynamicBody)
			filter.push_back(DYNAMIC_BODY_ID);
		if (_sensor)
			filter.push_back(SENSOR_ID);
		getManager().getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), &filter);
		
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getLinesSpriteBatch().end();

		if (_navigationGraph)
			getManager().getGameState().getNavigationGraph().draw();
		if (_grid)
			getManager().getGameState().getGrid().draw();
		
		drawFPS();
		debugLayerTimer.print("DEBUG LAYER");
	}

	const Hash DebugManager::TYPE("debug-manager");

	void DebugManager::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
		}
		else  if (message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if (key == Key::R)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLater().toggleSensor();
			}
			else if (key == Key::G)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLater().toggleGrid();
			}
			else if (key == Key::L)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLater().toggleSight();
			}
			else if (key == Key::B)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLater().toggleDynamicBody();
			}
			else if (key == Key::N)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLater().toggleNavigationGraph();
			}
		}
	}

	FXLayer::FXLayer(LayersManager* manager) : Layer(manager), _fbo1(Graphics::get().getFXSpriteBatch(), 10.0f), _fbo2(Graphics::get().getFXSpriteBatch(), 10.0f), _fxTimeUniform(0)
	{
		_fxTime = 0.0f;
		_fbo1.init();
		_fbo2.init();
		_fxTimeUniform = Graphics::get().getFXShaderProgram().getUniform("u_time");
	}

	void FXLayer::preDraw()
	{
		_fbo1.bind();
	}

	void FXLayer::draw(float framePeriod)
	{
		_fbo1.unbind();
		_fxTime += 1.0f/60.0f;
		Graphics::get().getFXShaderProgram().setUniform(_fxTimeUniform, _fxTime);
		Graphics::get().getFXShaderProgram().setUniform(Graphics::get().getFXSpriteBatch().getTypeUniform(), 1);
		_fbo2.bind();
		_fbo1.draw();
		_fbo2.unbind();
	}
	
	void FXLayer::draw2()
	{
		glBlendFunc(GL_ONE, GL_ONE);
		Graphics::get().getFXShaderProgram().setUniform(Graphics::get().getFXSpriteBatch().getTypeUniform(), 2);
		_fbo2.draw();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}