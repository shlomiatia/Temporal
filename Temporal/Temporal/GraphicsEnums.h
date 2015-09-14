#ifndef GRAPHICS_ENUMS_H
#define GRAPHICS_ENUMS_H

#include <vector>

namespace Temporal
{
	namespace LayerType
	{
		enum Enum
		{
			PARALLAX = 10,
			BACKGROUND = 20,
			BACKGROUND_PARTICLES = 23,
			DEBUG_LAYER = 27,
			STATIC = 30,
			PC = 40,
			COVER = 50,
			NPC = 60,
			LIGHTS = 65,
			FOREGROUND = 70
		};

		typedef std::vector<Enum> EnumList;
		typedef EnumList::const_iterator EnumIterator;

		static const EnumList List({
			PARALLAX,
			BACKGROUND,
			BACKGROUND_PARTICLES,
			DEBUG_LAYER,
			STATIC,
			PC,
			COVER,
			NPC,
			LIGHTS,
			FOREGROUND
		});
	}
}

#endif