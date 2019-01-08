#pragma once

#include "Layer.h"
#include "FBO.h"

namespace Temporal
{
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
}