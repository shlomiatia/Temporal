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
		void setCenter(const Vector& position) { _targetCenter = position; clamp(); };
		void translate(const Vector& movement) { _targetCenter += movement; clamp(); _center = _targetCenter; };

		Vector getBottomLeft() const;
		
	private:
		bool _followPlayer;
		Vector _center;
		Vector _targetCenter;
		

		void clamp();

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};

	class CameraControl : public Component
	{
	public:
		CameraControl() :  _shouldActivate(false){}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new CameraControl(); }

		static const Hash TYPE;
	private:
		static CameraControl* _active;
		bool _shouldActivate;
	};
}

#endif