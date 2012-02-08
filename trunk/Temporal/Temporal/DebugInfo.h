#pragma once
namespace Temporal
{
	class DebugInfo
	{
	public:
		static DebugInfo& get(void)
		{
			static DebugInfo instance;
			return (instance);
		}
		bool isShowingFPS(void) const { return _showingFPS; }
		void setShowingFPS(bool showingFPS) { _showingFPS = showingFPS; }

		void draw(void) const;
	private:
		bool _showingFPS;

		DebugInfo(void) : _showingFPS(false) {}
		~DebugInfo(void) {}
		DebugInfo(const DebugInfo&);
		DebugInfo& operator=(const DebugInfo&);
	};
}