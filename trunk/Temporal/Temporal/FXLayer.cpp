#include "FXLayer.h"
#include "Graphics.h"

namespace Temporal
{
	FXLayer::FXLayer(LayersManager* manager) : Layer(manager), _fbo1(Graphics::get().getFXSpriteBatch(), 10.0f), _fbo2(Graphics::get().getFXSpriteBatch(), 10.0f), _fxTimeUniform(0)
	{
		_fxTime = 0.0f;
		_fbo1.init();
		_fbo2.init();
		_fxTimeUniform = Graphics::get().getFXShaderProgram().getUniform("u_time");
	}
	void FXLayer::preDraw()
	{
		_fbo1.bind();
	}
	void FXLayer::draw(float framePeriod)
	{
		Graphics::get().getMatrixStack().top().reset();
		_fbo1.unbind();
		_fxTime += 1.0f / 60.0f;
	}
}