#ifndef SETTINGS_H
#define SETTINGS_H

#include "Vector.h"

namespace Temporal
{
	class Settings
	{
	public:
		Settings() {}

		const Vector& getResolution() const { return _resolution; }
		float getViewY() const { return _viewY; }
		bool isFullscreen() const { return _fullscreen; }
	private:
		Vector _resolution;
		float _viewY;
		bool _fullscreen;

		Settings(const Settings&);
		Settings& operator=(const Settings&);

		friend class SerializationAccess;
	};
}

#endif