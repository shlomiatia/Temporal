#ifndef LAYER_H
#define LAYER_H

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
			NPC = 4
		};

		static const int FARTHEST = BACKGROUND;
		static const int NEAREST = NPC;
	}

	class Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {};
		virtual void draw() = 0;

	private:
		Layer(const Layer&);
		Layer& operator=(const Layer&);
	};

	typedef std::vector<Layer*> LayerCollection;
	typedef LayerCollection::const_iterator LayerIterator;

	class LayersManager
	{
	public:
		static LayersManager& get()
		{
			static LayersManager instance;
			return (instance);
		}

		void add(Layer* layer) { _layers.push_back(layer); }
		void draw();
		void dispose();
	private:
		LayerCollection _layers;

		LayersManager() {}
		LayersManager(const LayersManager&);
		LayersManager& operator=(const LayersManager&);
	};

	class SpriteLayer : public Layer
	{
	public:
		void draw();
	private:
	};

	class DebugLayer : public Layer
	{
	public:
		void draw();
	};
}
#endif