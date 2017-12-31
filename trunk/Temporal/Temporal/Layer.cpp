#include "Layer.h"
#include "EntitySystem.h"
#include "Graphics.h"
#include "Hash.h"
#include "Timer.h"
#include "Camera.h"
#include "Lighting.h"
#include "InputEnums.h"
#include "DebugLayer.h"
#include "SpriteLayer.h"
#include <algorithm>
#include <SDL_opengl.h>

namespace Temporal
{
	void LayersManager::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);

		_camera = new Camera(this);
		_spriteLayer = new SpriteLayer(this);
		_debugLayer = new DebugLayer(this);
		_guiLayer = new GUILayer(this);
			
		_layers.push_back(_camera);
		_layers.push_back(_spriteLayer);
		if(_ambientColor != Color::White)
		{
			_lightLayer = new LightLayer(this, _ambientColor);
			_layers.push_back(_lightLayer);
		}
		_layers.push_back(_debugLayer);
		_layers.push_back(_guiLayer);

		//_fxLayer = new FXLayer(this);
		//_layers.push_back(_fxLayer);
	}


	LayersManager::~LayersManager()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			delete *i;
	}

	void LayersManager::draw(float framePeriod)
	{		

		if (_fxLayer)
			getFXLayer().preDraw();
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			(**i).draw(framePeriod);
		}
	}

	void LayersManager::addSprite(LayerType::Enum layer, Component* component)
	{
		_spriteLayer->add(layer, component); 
	}

	void LayersManager::removeSprite(LayerType::Enum layer, Component* component) 
	{
		_spriteLayer->remove(layer, component); 
	}

	void LayersManager::addLight(Component* component)
	{
		if(_lightLayer)
			_lightLayer->add(component);
	}

	void LayersManager::removeLight(Component* component)
	{
		if (_lightLayer)
			_lightLayer->remove(component);
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

	void GUILayer::remove(Component* component)
	{
		_components.erase(std::remove(_components.begin(), _components.end(), component));
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
		Graphics::get().getMatrixStack().top().reset();
		_fbo1.unbind();
		_fxTime += 1.0f/60.0f;
		Graphics::get().getFXShaderProgram().setUniform(_fxTimeUniform, _fxTime);
		Graphics::get().getFXShaderProgram().setUniform(Graphics::get().getFXSpriteBatch().getTypeUniform(), 0);

		_fbo2.bind();

		_fbo1.draw();

		_fbo2.unbind();
		getManager().getCamera().draw(framePeriod);
		getManager().getSpriteLayer().draw(framePeriod);
		Graphics::get().getMatrixStack().top().reset();
		getManager().getFXLayer().draw2();
	}
	
	void FXLayer::draw2()
	{
		glBlendFunc(GL_ONE, GL_ONE);
		Graphics::get().getFXShaderProgram().setUniform(Graphics::get().getFXSpriteBatch().getTypeUniform(), 2);
		_fbo2.draw();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}