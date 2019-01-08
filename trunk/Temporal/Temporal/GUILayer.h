#pragma once

#include "Layer.h"
#include "EntitySystem.h"
#include "FBO.h"
#include <vector>

namespace Temporal
{
	class GUILayer : public Layer

	{
	public:
		GUILayer(LayersManager* manager) : Layer(manager) {}
		void draw(float framePeriod);
		void add(Component* component) { _components.push_back(component); }
		void remove(Component* component);
		std::vector<Component*>& get() { return _components; }
	private:
		ComponentList _components;
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
}