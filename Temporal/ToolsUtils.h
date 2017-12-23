#ifndef TOOLS_UTILS_H
#define TOOLS_UTILS_H

namespace Temporal
{
	class ToolsUtils
	{
	public:
		static float snap(float val, float target, float maxSnap);
		static float snap(float val, float target, float maxSnap, float size);
	};
}
#endif