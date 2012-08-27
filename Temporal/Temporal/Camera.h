#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera(const Size& levelSize) : _levelSize(levelSize) {}
		void draw();
		
	private:
		Size _levelSize;

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif