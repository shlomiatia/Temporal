#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera(LayersManager* manager) : Layer(manager), _position(Vector::Zero) {}
		void draw();
		const Vector& getPosition() const { return _position; }
		
	private:
		Vector _position;

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif