#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera(LayersManager* manager, bool followPlayer) : Layer(manager), _followPlayer(followPlayer), _bottomLeft(Vector::Zero) {}
		void draw();
		
		void setFollowPlayer(bool followPlayer) { _followPlayer = followPlayer; }
		const Vector& getBottomLeft() const { return _bottomLeft; }
		void setCenter(const Vector& position);
		void setBottomLeft(const Vector& position);
		
	private:
		bool _followPlayer;
		Vector _bottomLeft;

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif