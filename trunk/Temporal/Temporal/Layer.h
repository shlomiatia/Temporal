#ifndef LAYER_H
#define LAYER_H

namespace Temporal
{
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

	
}
#endif