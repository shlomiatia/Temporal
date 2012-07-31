#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "NumericPair.h"

namespace Temporal
{
	class ViewManager
	{
	public:
		static ViewManager& get()
		{
			static ViewManager instance;
			return (instance);
		}

		const Point& getCameraBottomLeft() const { return _cameraBottomLeft; }
		void setLevelBounds(const Size& levelBounds) { _levelBounds = levelBounds; }

		void init(const Size& resolution, float logicalHeight, bool fullScreen = false);
		void setVideoMode(const Size& resolution, bool fullScreen = false);
		void update();

		
	private:
		Point _cameraBottomLeft;
		Size _levelBounds;
		Size _cameraSize;

		ViewManager() : _levelBounds(Size::Zero), _cameraSize(Size::Zero), _cameraBottomLeft(Vector::Zero) {}
		ViewManager(const ViewManager&);
		ViewManager& operator=(const ViewManager&);
	};
}

#endif