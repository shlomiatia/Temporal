#pragma  once

#include <Temporal\Base\Base.h>

namespace Temporal
{
	// TODO: Separate camera & resolution logics
	class ViewManager
	{
	public:
		static ViewManager& get(void)
		{
			static ViewManager instance;
			return (instance);
		}

		void setLevelBounds(const Vector& levelBounds) { _levelBounds = levelBounds; }

		void init(const Vector& resolution, float logicalHeight, bool fullScreen = false);
		void setVideoMode(const Vector& resolution, bool fullScreen = false);
		void setCameraCenter(const Vector& position);
	private:
		Vector _levelBounds;
		Vector _cameraSize;

		ViewManager(void) : _levelBounds(Vector::Zero), _cameraSize(Vector::Zero) {}
		ViewManager(const ViewManager&);
		ViewManager& operator=(const ViewManager&);
	};
}
