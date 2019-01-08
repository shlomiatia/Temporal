#pragma once

#include "EntitySystem.h"
#include "Color.h"
#include "GraphicsEnums.h"
#include "GUILayer.h"

namespace Temporal
{
	class Camera;
	class GUILayer;
	class SpriteLayer;
	class FXLayer;
	class DebugLayer;
	class LightLayer;
	class Layer;

	typedef std::vector<Layer*> LayerList;
	typedef LayerList::const_iterator LayerIterator;

	class LayersManager : public GameStateComponent
	{
	public:
		LayersManager() : _ambientColor(Color::White), _camera(0), _spriteLayer(0), _guiLayer(0), _lightLayer(0), _fxLayer(0), _debugLayer(0) {};
		~LayersManager();
		void init(GameState* gameState);
		void addSprite(LayerType::Enum layer, Component* component);
		void removeSprite(LayerType::Enum layer, Component* component);
		void addGUI(Component* component) { _guiLayer->add(component); }
		void removeGUI(Component* component) { _guiLayer->remove(component); }
		void addLight(Component* component);
		void removeLight(Component* component);
		Camera& getCamera() { return *_camera; }
		GUILayer& getGUILayer() { return *_guiLayer; }
		SpriteLayer& getSpriteLayer() { return *_spriteLayer; }
		FXLayer& getFXLayer() { return *_fxLayer; }
		DebugLayer& getDebugLayer() { return *_debugLayer; }
		LightLayer* getLightLayer() { return _lightLayer; }
		void draw(float framePeriod);
	private:
		Color _ambientColor;
		LayerList _layers;
		Camera* _camera;
		SpriteLayer* _spriteLayer;
		GUILayer* _guiLayer;
		LightLayer* _lightLayer;
		FXLayer* _fxLayer;
		DebugLayer* _debugLayer;
		LayersManager(const LayersManager&);
		LayersManager& operator=(const LayersManager&);
		friend class SerializationAccess;
	};
}