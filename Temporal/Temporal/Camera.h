#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "Vector.h"

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

		const Vector& getCameraBottomLeft() const { return _cameraBottomLeft; }
		void setLevelSize(const Size& levelSize) { _levelSize = levelSize; }

		void update();

		
	private:
		Vector _cameraBottomLeft;
		Size _levelSize;

		ViewManager() : _levelSize(Size::Zero), _cameraBottomLeft(Vector::Zero) {}
		ViewManager(const ViewManager&);
		ViewManager& operator=(const ViewManager&);
	};
}

#endif