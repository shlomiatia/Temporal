#ifndef LAYER_H
#define LAYER_H

#include "GameState.h"
#include "Vector.h"
#include "Color.h"
#include <vector>

namespace Temporal
{
	namespace LayerType
	{
		enum Enum
		{
			BACKGROUND = 0,
			STATIC = 1,
			PC = 2,
			COVER = 3,
			NPC = 4,
			PARTICLES = 5,
			GUI = 6
		};

		static const int FARTHEST = BACKGROUND;
		static const int NEAREST = PARTICLES;
	}

	class LayersManager;

	class Layer
	{
	public:
		Layer() : _manager(0) {}
		virtual ~Layer() {};
		virtual void draw() = 0;

		void init(LayersManager* manager) { _manager = manager; }

	protected:
		LayersManager& getLayersManager() { return *_manager; }

	private:
		LayersManager* _manager;

		Layer(const Layer&);
		Layer& operator=(const Layer&);
	};

	typedef std::vector<Layer*> LayerCollection;
	typedef LayerCollection::const_iterator LayerIterator;

	class LayersManager : public GameStateComponent
	{
	public:
		LayersManager() : _camera(false), _ambientColor(Color::White) {};
		~LayersManager();

		void init(GameState* gameState);

		void draw();
	private:
		Color _ambientColor;

		bool _camera;
		LayerCollection _layers;

		LayersManager(const LayersManager&);
		LayersManager& operator=(const LayersManager&);

		friend class SerializationAccess;
	};

	class SpriteLayer : public Layer
	{
	public:
		void draw();
	};

	class GUILayer : public Layer
	{
	public:
		void draw();
	};

	class DebugLayer : public Layer
	{
	public:
		void draw();
	private:
		void drawFPS();
	};
}
#endif