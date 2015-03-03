#ifndef LAYER_H
#define LAYER_H

#include "GameState.h"
#include "Vector.h"
#include "Color.h"
#include <vector>
#include <unordered_map>
#include "FBO.h"

namespace Temporal
{
	class LightLayer;

	namespace LayerType
	{
		enum Enum
		{
			BACKGROUND,
			STATIC,
			NPC,
			COVER,
			PC,
			PARTICLES,
			SIZE
		};
	}

	class Camera;
	class LayersManager;

	class Layer
	{
	public:
		Layer(LayersManager* manager) : _manager(manager) {}
		virtual ~Layer() {};
		virtual void draw() = 0;

	protected:
		LayersManager& getManager() { return *_manager; }

	private:
		LayersManager* _manager;

		Layer(const Layer&);
		Layer& operator=(const Layer&);
	};

	class Component;
	typedef std::unordered_map<LayerType::Enum, std::vector<Component*>> LayerComponentsMap;
	typedef LayerComponentsMap::const_iterator LayerComponentsIterator;

	class SpriteLayer : public Layer
	{
	public:
		SpriteLayer(LayersManager* manager);

		void draw();
		void add(LayerType::Enum layer, Component* component) { _layers.at(layer).push_back(component); }
		LayerComponentsMap& get() { return _layers; }

	private:
		LayerComponentsMap _layers;

		void innerDraw();
	};

	class GUILayer : public Layer
	{
	public:
		GUILayer(LayersManager* manager) : Layer(manager) {}

		void draw();
		void add(Component* component) { _components.push_back(component); }
		std::vector<Component*>& get() { return _components; }

	private:
		std::vector<Component*> _components;
	};

	class DebugLayer : public Layer
	{
	public:
		DebugLayer(LayersManager* manager) : Layer(manager) {}

		void draw();
	private:
		void drawFPS();
	};

	class FXLayer : public Layer
	{
	public:
		FXLayer(LayersManager* manager);

		void preDraw();
		void draw();
		void draw2();
	private:
		int _fxTimeUniform;
		float _fxTime;
		FBO _fbo1;
		FBO _fbo2;
	};

	typedef std::vector<Layer*> LayerCollection;
	typedef LayerCollection::const_iterator LayerIterator;

	class LayersManager : public GameStateComponent
	{
	public:
		LayersManager() : _cameraFollowPlayer(false), _ambientColor(Color::White), _camera(0), _spriteLayer(0), _guiLayer(0), _lightLayer(0), _fxLayer(0) {};
		~LayersManager();

		void init(GameState* gameState);
		void addSprite(LayerType::Enum layer, Component* component) { _spriteLayer->add(layer, component); }
		void addGUI(Component* component) { _guiLayer->add(component); }
		void addLight(Component* component);
		Camera& getCamera() { return *_camera; }
		GUILayer& getGUILayer() { return *_guiLayer; }
		SpriteLayer& getSpriteLayer() { return *_spriteLayer; }
		FXLayer& getFXLayer() { return *_fxLayer; }
		void draw();

	private:
		bool _cameraFollowPlayer;
		Color _ambientColor;

		LayerCollection _layers;
		Camera* _camera;
		SpriteLayer* _spriteLayer;
		GUILayer* _guiLayer;
		LightLayer* _lightLayer;
		FXLayer* _fxLayer;

		LayersManager(const LayersManager&);
		LayersManager& operator=(const LayersManager&);

		friend class SerializationAccess;
	};
}
#endif