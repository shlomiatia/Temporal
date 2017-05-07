#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"
#include "EntitySystem.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera(LayersManager* manager, bool followPlayer);

		void draw(float framePeriod);
		
		void setFollowPlayer(bool followPlayer) { _followPlayer = followPlayer; }
		bool isFollowPlayer() const { return _followPlayer;  }
		void setCenter(const Vector& position) { _targetCenter = position; clamp(); };
		const Vector& getCenter() const { return _targetCenter; }
		void translate(const Vector& movement) { _targetCenter += movement; clamp(); _center = _targetCenter; };
		void setActiveCameraControl(Hash activeCameraControl) { _activeCameraControl = activeCameraControl; }
		Hash getActiveCameraControl() const { return _activeCameraControl; }

		Vector getBottomLeft() const;
		
	private:
		bool _followPlayer;
		bool _foundPlayer;
		Vector _center;
		Vector _targetCenter;	
		Hash _activeCameraControl;

		void clamp();

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};

	class CameraControl : public Component
	{
	public:

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new CameraControl(); }

		static const Hash TYPE;
	private:
	};
}

#endif