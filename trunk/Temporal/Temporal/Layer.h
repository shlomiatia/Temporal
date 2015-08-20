#ifndef LAYER_H
#define LAYER_H

#include "GameState.h"
#include "Vector.h"
#include "Color.h"
#include "FBO.h"
#include "EntitySystem.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>

namespace Temporal
{
	class LightLayer;

	namespace LayerType
	{
		enum Enum
		{
			PARALLAX,	// 0
			BACKGROUND,	// 1
			STATIC,		// 2
			PC,			// 3
			COVER,		// 4
			NPC,		// 5
			PARTICLES,	// 6
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
		virtual void draw(float framePeriod) = 0;

	protected:
		LayersManager& getManager() { return *_manager; }
		const LayersManager& getManager() const { return *_manager; }

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

		void draw(float framePeriod);
		void add(LayerType::Enum layer, Component* component) { _layers.at(layer).push_back(component); }
		void remove(LayerType::Enum layer, Component* component);
		LayerComponentsMap& get() { return _layers; }

	private:
		LayerComponentsMap _layers;

		void innerDraw();
	};

	class GUILayer : public Layer
	{
	public:
		GUILayer(LayersManager* manager) : Layer(manager) {}

		void draw(float framePeriod);
		void add(Component* component) { _components.push_back(component); }
		std::vector<Component*>& get() { return _components; }

	private:
		std::vector<Component*> _components;
	};

	class DebugLayer : public Layer
	{
	public:
		DebugLayer(LayersManager* manager) : 
			Layer(manager), _staticBody(false), _sight(false), _dynamicBody(false), _sensor(false), _grid(false), _navigationGraph(false), _cameraControl(false), _notifying(false) {}

		void draw(float framePeriod);

		void toggleStaticBody() { _staticBody = !_staticBody;  }
		void toggleSight() { _sight = !_sight; }
		void toggleDynamicBody() { _dynamicBody = !_dynamicBody; }
		void toggleSensor() { _sensor = !_sensor; }
		void toggleGrid() { _grid = !_grid; }
		void toggleNavigationGraph() { _navigationGraph = !_navigationGraph; }
		void toggleCameraControl() { _cameraControl = !_cameraControl; }
		bool isCameraControl() const { return _cameraControl; }
		void showInfo(const char* info);
		void notify(const char* notification);
	private:
		void drawFPS();

		bool _staticBody;
		bool _sight;
		bool _dynamicBody;
		bool _sensor;
		bool _grid;
		bool _navigationGraph;
		bool _cameraControl;

		bool _notifying;
		Timer _notificationTimer;
	};

	class DebugManager : public Component
	{
	public:
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new DebugManager(); }

		static const Hash TYPE;
	private:
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
		LayersManager() : _cameraFollowPlayer(false), _ambientColor(Color::White), _camera(0), _spriteLayer(0), _guiLayer(0), _lightLayer(0), _fxLayer(0), _debugLayer(0){};
		~LayersManager();

		void init(GameState* gameState);
		void addSprite(LayerType::Enum layer, Component* component) { _spriteLayer->add(layer, component); }
		void removeSprite(LayerType::Enum layer, Component* component) { _spriteLayer->remove(layer, component); }
		void addGUI(Component* component) { _guiLayer->add(component); }
		void addLight(Component* component);
		void removeLight(Component* component);
		Camera& getCamera() { return *_camera; }
		GUILayer& getGUILayer() { return *_guiLayer; }
		SpriteLayer& getSpriteLayer() { return *_spriteLayer; }
		FXLayer& getFXLayer() { return *_fxLayer; }
		DebugLayer& getDebugLater() { return *_debugLayer; }
		LightLayer* getLightLayer() { return _lightLayer; }
		void draw(float framePeriod);

	private:
		bool _cameraFollowPlayer;
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