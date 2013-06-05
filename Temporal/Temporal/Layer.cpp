#include "Layer.h"
#include "EntitySystem.h"
#include "Graphics.h"
#include "Thread.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include <sstream>

namespace Temporal
{
	void LayersManager::draw()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
			(**i).draw();
	}

	void LayersManager::dispose()
	{
		for(LayerIterator i = _layers.begin(); i != _layers.end(); ++i)
		{
			delete *i;
		}
	}

	void SpriteLayer::draw()
	{
		for(int i = LayerType::FARTHEST; i <= LayerType::NEAREST; ++i)
			EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW, &i));
	}

	void DebugLayer::draw()
	{
		ComponentType::Enum filter = ComponentType::STATIC_BODY;
		EntitiesManager::get().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), filter);
		
		//Grid::get().draw();
		NavigationGraph::get().draw();

		std::ostringstream title;
		static const int MAX_SAMPLES = 100;
		static float samples[MAX_SAMPLES];
		static int sampleIndex = 0;
		static float sum = 0.0f;
		static float lastTick = 0.0f;
		static float newSample = 0;

		if (lastTick == 0)
		{
			for (int i = 0; i < MAX_SAMPLES; i++)
				samples[i] = 0;
		}
		else
		{
			newSample = Thread::getElapsedTime() - lastTick;
			sum += newSample - samples[sampleIndex];
			samples[sampleIndex] = newSample;
			sampleIndex = (sampleIndex + 1) % MAX_SAMPLES;

			title << "FPS: " << MAX_SAMPLES / sum;
		}
		lastTick = Thread::getElapsedTime();
		Graphics::get().setTitle(title.str().c_str());
	}
}