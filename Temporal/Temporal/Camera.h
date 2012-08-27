#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"

namespace Temporal
{
	class Camera
	{
	public:
		static Camera& get()
		{
			static Camera instance;
			return (instance);
		}

		const Vector& getCameraBottomLeft() const { return _cameraBottomLeft; }
		void setLevelSize(const Size& levelSize) { _levelSize = levelSize; }

		void update();

		
	private:
		Vector _cameraBottomLeft;
		Size _levelSize;

		Camera() : _levelSize(Size::Zero), _cameraBottomLeft(Vector::Zero) {}
		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif