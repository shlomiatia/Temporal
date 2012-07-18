#ifndef PHYSICSUTILS_H
#define PHYSICSUTILS_H

namespace Temporal
{
	inline bool canCollide(int sourceFilter, int targetFilter)
	{
		return sourceFilter == 0 || targetFilter == 0 || (sourceFilter & targetFilter) != 0;
	}
}

#endif