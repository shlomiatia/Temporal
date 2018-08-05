#include "SpriteLayer.h"
#include "Graphics.h"
#include "Camera.h"
#include "LayersManager.h"
#include "Timer.h"
#include <algorithm>

namespace Temporal
{
	SpriteLayer::SpriteLayer(LayersManager* manager) : Layer(manager)
	{
		for (LayerType::EnumIterator i = LayerType::List.begin(); i != LayerType::List.end(); ++i)
			_layers[*i] = ComponentList();
	}

	void SpriteLayer::preDraw(LayerType::Enum layer)
	{
		if (layer == LayerType::PARALLAX)
		{
			Graphics::get().getMatrixStack().top().translate(getManager().getCamera().getBottomLeft() * 0.9f);
		}

		if (layer != LayerType::BACKGROUND_PARTICLES)
		{
			Graphics::get().getSpriteBatch().begin();
		}
	}
	
	void SpriteLayer::postDraw(LayerType::Enum layer)
	{
		if (layer != LayerType::BACKGROUND_PARTICLES)
		{
			Graphics::get().getSpriteBatch().end();
		}

		if (layer == LayerType::PARALLAX)
		{
			Graphics::get().getMatrixStack().top().translate(-getManager().getCamera().getBottomLeft() * 0.9f);
		}
	}

	void SpriteLayer::innerDraw()
	{
		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), 0);
		for (LayerType::EnumIterator i = LayerType::List.begin(); i != LayerType::List.end(); ++i)
		{
			LayerType::Enum layer = *i;
			
			preDraw(layer);
			
			ComponentList& components = _layers.at(layer);
			for (ComponentIterator j = components.begin(); j != components.end(); ++j)
			{
				(**j).handleMessage(Message(MessageID::DRAW));
			}

			postDraw(layer);
		}

	}

	PerformanceTimer& spriteLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_SPRITE_LAYER"));
	void SpriteLayer::draw(float framePeriod)
	{
		spriteLayerTimer.measure();
		innerDraw();
		spriteLayerTimer.print("SPRITE LAYER");
	}

	void SpriteLayer::remove(LayerType::Enum layer, Component* component)
	{
		std::vector<Component*>& components = _layers.at(layer);
		components.erase(std::remove(components.begin(), components.end(), component));
	}
}