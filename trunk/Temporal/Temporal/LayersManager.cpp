#include "LayersManager.h"
#include "Camera.h"
#include "SpriteLayer.h"
#include "DebugLayer.h"
#include "GUILayer.h"
#include "Lighting.h"

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
		if (_ambientColor != Color::White)
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
		for (LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			delete *i;
	}
	void LayersManager::draw(float framePeriod)
	{
		if (_fxLayer)
			getFXLayer().preDraw();
		for (LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
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
		if (_lightLayer)
			_lightLayer->add(component);
	}
	void LayersManager::removeLight(Component* component)
	{
		if (_lightLayer)
			_lightLayer->remove(component);
	}
}