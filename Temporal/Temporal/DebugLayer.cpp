#include "DebugLayer.h"
#include "Graphics.h"
#include "Grid.h"
#include "NavigationGraph.h"
#include "Hash.h"
#include "Timer.h"
#include "InputEnums.h"
#include "Renderer.h"
#include "PhysicsEnums.h"
#include "Camera.h"

namespace Temporal
{
	PerformanceTimer& debugLayerTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_DEBUG_LAYER"));

	static const Hash FPS_TIMER = Hash("TMR_FPS");

	static const Hash STATIC_BODY_ID("static-body");
	static const Hash DYNAMIC_BODY_ID("dynamic-body");
	static const Hash SENSOR_ID("sensor");
	static const Hash SIGHT_ID("sight");
	static const Hash NAVIGATOR_ID("navigator");
	static const Hash CAMERA_CONTROL_ID("camera-control");
	static const Hash LIGHT_ID("light");

	static const Hash EDITABLE_ID("editable");

	DebugLayer::DebugLayer(LayersManager* manager) : Layer(manager), _grid(false), _navigationGraph(false), _notifying(false), _camera(false)
	{
		_componentsDebugInfo.push_back(new ComponentDebugInfo(STATIC_BODY_ID, Key::T, false, "resources/textures/patrol-control.png"));
		_componentsDebugInfo.push_back(new ComponentDebugInfo(DYNAMIC_BODY_ID, Key::Y));
		_componentsDebugInfo.push_back(new ComponentDebugInfo(SENSOR_ID, Key::R));
		_componentsDebugInfo.push_back(new ComponentDebugInfo(SIGHT_ID, Key::U));
		_componentsDebugInfo.push_back(new ComponentDebugInfo(CAMERA_CONTROL_ID, Key::I, false, "resources/textures/camera-control.png"));
	}

	DebugLayer::~DebugLayer()
	{
		for (ComponentDebugInfoIterator i = _componentsDebugInfo.begin(); i != _componentsDebugInfo.end(); ++i)
		{
			delete *i;
		}
	}

	void DebugLayer::drawFPS()
	{
		PerformanceTimer& timer = PerformanceTimerManager::get().getTimer(FPS_TIMER);
		timer.print("=== FPS ===");
		timer.measure();
	}

	void DebugLayer::showInfo(const char* info)
	{
		if (!_notifying)
			Graphics::get().setTitle(info);
	}


	void DebugLayer::notify(const char* notification)
	{
		Graphics::get().setTitle(notification);
		_notificationTimer.reset();
		_notifying = true;
	}

	void DebugLayer::draw(float framePeriod)
	{
		debugLayerTimer.measure();
		Graphics::get().getLinesSpriteBatch().begin();

		Graphics::get().getShaderProgram().setUniform(Graphics::get().getSpriteBatch().getTypeUniform(), -1);
		HashList filter;

		filter.push_back(EDITABLE_ID);
		filter.push_back(ComponentsIds::RENDERER);
		for (ComponentDebugInfoIterator i = _componentsDebugInfo.begin(); i != _componentsDebugInfo.end(); ++i)
		{
			ComponentDebugInfo&  info = **i;
			if (info.isDebugging())
				filter.push_back(info.getId());
		}
		if (_navigationGraph)
			filter.push_back(NAVIGATOR_ID);
		getManager().getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_DEBUG), &filter);

		if (_navigationGraph)
		{
			getManager().getGameState().getNavigationGraph().drawDebug();
		}
			
		if (_grid)
		{
			getManager().getGameState().getGrid().drawDebug();
		}

		Graphics::get().getLinesSpriteBatch().end();


		drawFPS();

		if (_notifying)
		{
			_notificationTimer.update(framePeriod);
			if (_notificationTimer.getElapsedTime() > 1.0f)
			{
				_notifying = false;
				Graphics::get().setTitle("");
			}
		}

		debugLayerTimer.print("DEBUG LAYER");
	}

	bool isNonPatrolControlStaticBody(const ComponentDebugInfo& info, const Entity& entity)
	{
		if (info.getId() == STATIC_BODY_ID)
		{
			CollisionCategory::Enum category = *static_cast<CollisionCategory::Enum*>(entity.handleMessage(Message(MessageID::GET_COLLISION_CATEGORY)));
			if (category != CollisionCategory::PATROL_CONTROL)
				return true;
		}
		return false;
	}

	void DebugManager::addDebugRendererToEntity(Entity& entity)
	{
		ComponentDebugInfoList& list = getEntity().getManager().getGameState().getLayersManager().getDebugLayer().getComponentsDebugInfo();
		for (ComponentDebugInfoIterator i = list.begin(); i != list.end(); ++i)
		{
			ComponentDebugInfo& info = **i;
			if (info.getTexture() && entity.get(info.getId()) && !isNonPatrolControlStaticBody(info, entity))
			{
				Renderer* renderer = new Renderer(info.getTexture(), "", 0, LayerType::DEBUG_LAYER, Color::White, info.isDebugging());
				renderer->setBypassSave(true);
				entity.add(renderer);
			}
		}
	}

	void DebugManager::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().setBypassSave(true);
			getEntity().getManager().addInputComponent(this);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else  if (message.getID() == MessageID::ENTITY_READY)
		{
			HashEntityMap& entities = getEntity().getManager().getEntities();
			for (HashEntityIterator j = entities.begin(); j != entities.end(); ++j)
			{
				Entity& entity = *(j->second);
				addDebugRendererToEntity(entity);
			}
			if (_startInDebugMode)
			{
				ComponentDebugInfoList& list = getEntity().getManager().getGameState().getLayersManager().getDebugLayer().getComponentsDebugInfo();
				for (ComponentDebugInfoIterator i = list.begin(); i != list.end(); ++i)
				{
					ComponentDebugInfo&  info = **i;
					if (info.getId() == STATIC_BODY_ID || info.getId() == CAMERA_CONTROL_ID || info.getId() == LIGHT_ID)
						toggleDebugging(info);
				}
			}
			
		}
		else  if (message.getID() == MessageID::KEY_UP)
		{

			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			ComponentDebugInfoList& list = getEntity().getManager().getGameState().getLayersManager().getDebugLayer().getComponentsDebugInfo();
			for (ComponentDebugInfoIterator i = list.begin(); i != list.end(); ++i)
			{
				ComponentDebugInfo&  info = **i;
				if (info.getKey() == key)
				{
					toggleDebugging(info);
				}
			}
			if (key == Key::G)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLayer().toggleGrid();
			}
			else if (key == Key::N)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLayer().toggleNavigationGraph();
			}
			else if (key == Key::C)
			{
				getEntity().getManager().getGameState().getLayersManager().getDebugLayer().toggleCamera();
			}
		}
	}

	void DebugManager::toggleDebugging(ComponentDebugInfo& info)
	{
		info.toggleDebugging();
		if (info.getTexture())
		{
			HashEntityMap& entities = getEntity().getManager().getEntities();
			for (HashEntityIterator j = entities.begin(); j != entities.end(); ++j)
			{
				Entity& entity = *(j->second);
				if (entity.get(info.getId()) && !isNonPatrolControlStaticBody(info, entity))
				{
					ComponentList renderers = entity.getAll(ComponentsIds::RENDERER);
					for (ComponentIterator k = renderers.begin(); k != renderers.end(); ++k)
					{
						if ((**k).isBypassSave())
						{
							bool isDebugging = info.isDebugging();
							(**k).handleMessage(Message(MessageID::SET_VISIBILITY, &isDebugging));
						}
					}

				}
			}
		}
	}
}
