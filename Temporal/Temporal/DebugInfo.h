#ifndef DEBUGINFO_H
#define DEBUGINFO_H

namespace Temporal
{
	class DebugInfo
	{
	public:
		static DebugInfo& get()
		{
			static DebugInfo instance;
			return (instance);
		}
		bool isShowingFPS() const { return _showingFPS; }
		void setShowingFPS(bool showingFPS) { _showingFPS = showingFPS; }

		void draw() const;
	private:
		bool _showingFPS;

		DebugInfo() : _showingFPS(false) {}
		~DebugInfo() {}
		DebugInfo(const DebugInfo&);
		DebugInfo& operator=(const DebugInfo&);
	};
}
#endif