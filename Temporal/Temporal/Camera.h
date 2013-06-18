#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera() {}
		void draw();
		
	private:
		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif