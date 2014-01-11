#ifndef LAYER_H
#define LAYER_H

#include "GameState.h"
#include "Vector.h"
#include "Color.h"
#include <vector>
#include <unordered_map>

namespace Temporal
{
	namespace LayerType
	{
		enum Enum
		{
			BACKGROUND,
			STATIC,
			PC,
			COVER,
			NPC,
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

	typedef std::vector<Layer*> LayerCollection;
	typedef LayerCollection::const_iterator LayerIterator;

	class LayersManager : public GameStateComponent
	{
	public:
		LayersManager() : _useCamera(false), _ambientColor(Color::White), _camera(0), _spriteLayer(0), _guiLayer(0) {};
		~LayersManager();

		void init(GameState* gameState);
		void addSprite(LayerType::Enum layer, Component* component) { _spriteLayer->add(layer, component); }
		void addGUI(Component* component) { _guiLayer->add(component); }
		Camera& getCamera() { return *_camera; }
		GUILayer& getGUILayer() { return *_guiLayer; }
		SpriteLayer& getSpriteLayer() { return *_spriteLayer; }
		void draw();

	private:
		Color _ambientColor;

		bool _useCamera;
		LayerCollection _layers;
		Camera* _camera;
		SpriteLayer* _spriteLayer;
		GUILayer* _guiLayer;

		LayersManager(const LayersManager&);
		LayersManager& operator=(const LayersManager&);

		friend class SerializationAccess;
	};
}
#endif