#ifndef LAYER_H
#define LAYER_H

#include "GameState.h"
#include "Vector.h"
#include "Color.h"
#include "FBO.h"
#include "EntitySystem.h"
#include "Timer.h"
#include "InputEnums.h"
#include "GraphicsEnums.h"
#include <vector>

namespace Temporal
{
	class LightLayer;
	class DebugLayer;
	class Camera;
	class LayersManager;
	class SpriteLayer;

	class Layer
	{
	public:
		Layer(LayersManager* manager) : _manager(manager) {}
		virtual ~Layer() {};
		virtual void draw(float framePeriod) = 0;

	protected:
		LayersManager& getManager() { return *_manager; }
		const LayersManager& getManager() const { return *_manager; }

	private:
		LayersManager* _manager;

		Layer(const Layer&);
		Layer& operator=(const Layer&);
	};

	class GUILayer : public Layer
	{
	public:
		GUILayer(LayersManager* manager) : Layer(manager) {}

		void draw(float framePeriod);
		void add(Component* component) { _components.push_back(component); }
		void remove(Component* component);
		std::vector<Component*>& get() { return _components; }

	private:
		ComponentList _components;
	};

	class FXLayer : public Layer
	{
	public:
		FXLayer(LayersManager* manager);

		void preDraw();
		void draw(float framePeriod);
	private:
		int _fxTimeUniform;
		float _fxTime;
		FBO _fbo1;
		FBO _fbo2;

		
		void draw2();
	};

	typedef std::vector<Layer*> LayerList;
	typedef LayerList::const_iterator LayerIterator;

	class LayersManager : public GameStateComponent
	{
	public:
		LayersManager() : _ambientColor(Color::White), _camera(0), _spriteLayer(0), _guiLayer(0), _lightLayer(0), _fxLayer(0), _debugLayer(0){};
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
#endif