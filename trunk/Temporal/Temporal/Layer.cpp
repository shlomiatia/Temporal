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
#include "Lighting.h"
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
		_fxLayer = new FXLayer(this);
			
		_layers.push_back(_camera);
		_layers.push_back(_spriteLayer);
		if(_ambientColor != Color::White)
		{
			_lightLayer = new LightLayer(this, _ambientColor);
			_layers.push_back(_lightLayer);
		}
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

	void LayersManager::addLight(Component* component)
	{
		if(_lightLayer)
			_lightLayer->add(component);
	}

	SpriteLayer::SpriteLayer(LayersManager* manager) : Layer(manager)
	{
		for(int i = 0; i <= LayerType::SIZE; ++i)
			_layers[static_cast<LayerType::Enum>(i)] = ComponentCollection();
	}

	void SpriteLayer::innerDraw()
	{
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
		Graphics::get().getSpriteBatch().end();
	}

	PerformanceTimer& spriteLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_SPRITE_LAYER"));
	void SpriteLayer::draw()
	{
		spriteLayerTimer.measure();
		//getManager().getFXLayer().preDraw();
		innerDraw();
		/*getManager().getFXLayer().draw();
		innerDraw();
		getManager().getFXLayer().draw2();*/
		spriteLayerTimer.print("SPRITE LAYER");
	}

	PerformanceTimer& guiLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_GUI_LAYER"));
	void GUILayer::draw()
	{
		guiLayerTimer.measure();
		Graphics::get().getMatrixStack().reset();
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
		timer.print("FPS");
		timer.measure();
	}

	PerformanceTimer& debugLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_DEBUG_LAYER"));
	void DebugLayer::draw()
	{
		debugLayerTimer.measure();
		Graphics::get().getSpriteBatch().begin();

		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), -1);
		HashCollection filter;
		static const Hash STATIC_BODY_STYPE = Hash("static-body");
		static const Hash SIGHT_TYPE = Hash("sight");
		filter.push_back(STATIC_BODY_STYPE);
		filter.push_back(SIGHT_TYPE);
		getManager().getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), &filter);
		
		getManager().getGameState().getGrid().draw();
		//NavigationGraph::get().draw();

		Graphics::get().getSpriteBatch().end();
		drawFPS();
		debugLayerTimer.print("DEBUG LAYER");
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

	void FXLayer::draw()
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