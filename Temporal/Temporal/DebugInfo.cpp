#include "DebugInfo.h"

#include "Thread.h"
#include "Graphics.h"
#include <sstream>

namespace Temporal
{
	void DebugInfo::draw(void) const
	{
		if (isShowingFPS())
		{
			std::ostringstream title;
			static const int MAX_SAMPLES = 100;
			static int samples[MAX_SAMPLES];
			static int sum = 0;
			static int sampleIndex = 0;
			static int lastTick = 0;
			static int newSample = 0;

			if (lastTick == 0)
			{
				for (int i = 0; i < MAX_SAMPLES; i++)
					samples[i] = 0;
			}
			else
			{
				
				newSample = Thread::getElapsedTimeInMillis() - lastTick;
				sum += newSample - samples[sampleIndex];
				samples[sampleIndex] = newSample;
				sampleIndex = (sampleIndex + 1) % MAX_SAMPLES;

				title << "FPS: " << 1000.0f / ((float)sum / MAX_SAMPLES);
				
			}
			lastTick = Thread::getElapsedTimeInMillis();
			Graphics::get().setTitle(title.str().c_str());
		}
	}
}