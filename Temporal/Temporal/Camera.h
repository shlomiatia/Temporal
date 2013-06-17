#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera(const Vector& levelSize) : _levelSize(levelSize) {}
		void draw();
		
	private:
		Vector _levelSize;

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif