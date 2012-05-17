#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "NumericPair.h"

namespace Temporal
{
	class ViewManager
	{
	public:
		static ViewManager& get(void)
		{
			static ViewManager instance;
			return (instance);
		}

		void setLevelBounds(const Size& levelBounds) { _levelBounds = levelBounds; }

		void init(const Size& resolution, float logicalHeight, bool fullScreen = false);
		void setVideoMode(const Size& resolution, bool fullScreen = false);
		void update(void) const;
	private:
		Size _levelBounds;
		Size _cameraSize;

		ViewManager(void) : _levelBounds(Size::Zero), _cameraSize(Size::Zero) {}
		ViewManager(const ViewManager&);
		ViewManager& operator=(const ViewManager&);
	};
}

#endif