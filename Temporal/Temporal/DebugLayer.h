#ifndef DEBUG_LAYER_H
#define DEBUG_LAYER_H

#include "InputEnums.h"
#include "Layer.h"
#include <vector>

namespace Temporal
{
	class ComponentDebugInfo
	{
	public:
		ComponentDebugInfo(Hash id, Key::Enum key, bool isDebugging = false, const char* texture = 0) : _id(id), _key(key), _isDebugging(isDebugging), _texture(texture) {}

		Hash getId() const { return _id; }
		Key::Enum getKey() const { return _key; }
		const char* getTexture() const { return _texture; }
		bool isDebugging() const { return _isDebugging; }
		void toggleDebugging() { _isDebugging = !_isDebugging; }

	private:
		Hash _id;
		Key::Enum _key;
		bool _isDebugging;
		const char* _texture;

		ComponentDebugInfo(const ComponentDebugInfo&);
		ComponentDebugInfo& operator=(const ComponentDebugInfo&);
	};

	typedef std::vector<ComponentDebugInfo*> ComponentDebugInfoList;
	typedef ComponentDebugInfoList::const_iterator ComponentDebugInfoIterator;

	class DebugLayer : public Layer
	{
	public:
		DebugLayer(LayersManager* manager);
		~DebugLayer();

		void draw(float framePeriod);

		void toggleGrid() { _grid = !_grid; }
		void toggleNavigationGraph() { _navigationGraph = !_navigationGraph; }

		void showInfo(const char* info);
		void notify(const char* notification);
		ComponentDebugInfoList& getComponentsDebugInfo() { return _componentsDebugInfo; }

	private:
		void drawFPS();

		bool _grid;
		bool _navigationGraph;

		ComponentDebugInfoList _componentsDebugInfo;

		bool _notifying;
		Timer _notificationTimer;
	};

	class DebugManager : public Component
	{
	public:
		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new DebugManager(); }

		void addDebugRendererToEntity(Entity& entity);

		static const Hash TYPE;
	private:
	};
}
#endif