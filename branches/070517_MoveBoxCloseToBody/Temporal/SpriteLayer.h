#ifndef SPRITE_LAYER_H
#define SPRITE_LAYER_H

#include "Layer.h"
#include "GraphicsEnums.h"
#include <unordered_map>

namespace Temporal
{
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
		void preDraw(LayerType::Enum layer);
		void postDraw(LayerType::Enum layer);
	};
}
#endif