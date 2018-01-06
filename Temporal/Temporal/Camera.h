#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Layer.h"
#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class Camera : public Layer
	{
	public:
		Camera(LayersManager* manager);

		void draw(float framePeriod);
		void drawDebug();
		
		void setTrackedEntityId(Hash trackedEntityId);
		Hash getTrackedEntityId() const { return _trackedEntityId; }
		void translate(const Vector& movement) { _center += movement; clamp(); };
		Vector getBottomLeft() const;
		
	private:
		Hash _trackedEntityId;
		Vector _center;
		Vector _previousCenter;
		Vector _targetCenter;
		Vector _window;
		float _t;

		void clamp();
		void handleWindow(const Vector& playerPosition);

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};

	class CameraControl : public Component
	{
	public:

		Hash getType() const { return ComponentsIds::CAMERA_CONTROL; }
		void handleMessage(Message& message);

		Component* clone() const { return new CameraControl(); }

	};
}

#endif