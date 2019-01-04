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
		
		void setTrackedEntityId(Hash trackedEntityId);
		Hash getTrackedEntityId() const { return _trackedEntityId; }
		void translate(const Vector& movement) { _center += movement; clamp(); };
		void scale(float scale);
		void resetScale() { _scale = 1.0f; }
		
		Vector getOffsetPosition(const Vector& vector) const;
		Vector getParallaxPosition(float parallaxScale) const;
		Vector getUnscaledBottomLeft() const;

	private:
		Hash _trackedEntityId;
		Vector _center; 
		Vector _previousCenter;
		Vector _targetCenter;
		Vector _window;
		float _time;
		float _scale;

		void clamp();
		void handleWindow(const Vector& playerPosition);
		Vector getBottomLeft() const;

		Camera(const Camera&);
		Camera& operator=(const Camera&);
	};
}

#endif