#include "Mouse.h"
#include "Graphics.h"
#include "GameState.h"
#include "MessageUtils.h"
#include "EntitySystem.h"
#include "SpriteLayer.h"
#include "Camera.h"
#include "LayersManager.h"
#include "Lighting.h"
#include "GUILayer.h"

#include <SDL.h>

namespace Temporal
{
	void fillPosition(SDL_Event& e, Vector& position)
	{
		float x = e.button.x * Graphics::get().getLogicalView().getX() / Graphics::get().getResolution().getX();
		float y = Graphics::get().getLogicalView().getY() - e.button.y * Graphics::get().getLogicalView().getY() / Graphics::get().getResolution().getY();
		position = Vector(x, y);
	}

	Mouse::Mouse()
	{
		_buttonsMap[SDL_BUTTON_LEFT] = MouseButton::LEFT;
		_buttonsMap[SDL_BUTTON_MIDDLE] = MouseButton::MIDDLE;
		_buttonsMap[SDL_BUTTON_RIGHT] = MouseButton::RIGHT;

		for(IntMouseButtonIterator i = _buttonsMap.begin(); i != _buttonsMap.end(); ++i)
		{
			_buttons[i->second] = false;
		}
		
	}

	Vector Mouse::getOffsetPosition() const
	{
		Camera& camera = GameStateManager::get().getCurrentState().getLayersManager().getCamera();
		Vector offsetPosition = camera.getOffsetPosition(getPosition());

		return offsetPosition;
	}

	void Mouse::sendMessage(MessageID::Enum messageId, MouseParams& params)
	{
		Vector offsetPosition = getOffsetPosition();
		MouseParams offsetParams(params.getButton(), offsetPosition);
		Message offsetMessage(messageId, &offsetParams);
		Component* focusComponent = GameStateManager::get().getCurrentState().getEntitiesManager().getFocusInputComponent();
		if (focusComponent)
		{ 
			focusComponent->raiseMessage(offsetMessage);
		}
		else
		{
			LayersManager& layersManager = GameStateManager::get().getCurrentState().getLayersManager();
			Message message(messageId, &params);

			ComponentList& guiComponents = layersManager.getGUILayer().get();
			for (ComponentIterator j = guiComponents.begin(); j != guiComponents.end(); ++j)
			{
				(**j).raiseMessage(message);
				if (params.isHandled())
					return;
			}

			LightLayer* lightLayer = layersManager.getLightLayer();
			if (lightLayer)
			{
				ComponentList& lightComponents = lightLayer->get();
				for (ComponentIterator j = lightComponents.begin(); j != lightComponents.end(); ++j)
				{
					(**j).raiseMessage(offsetMessage);
					if (params.isHandled())
						return;
				}
			}

			LayerComponentsMap& layerComponentsMap = layersManager.getSpriteLayer().get();
			for (LayerType::EnumList::const_reverse_iterator i = LayerType::List.rbegin(); i != LayerType::List.rend(); ++i)
			{
				ComponentList& components = layerComponentsMap.at(*i);
				for (ComponentIterator j = components.begin(); j != components.end(); ++j)
				{
					(**j).raiseMessage(offsetMessage);
					if (offsetParams.isHandled())
						return;
				}
			}
		}
	}

	void Mouse::dispatchEvent(void* obj)
	{
		SDL_Event& e = *static_cast<SDL_Event*>(obj);
		
		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			MouseButton::Enum button = _buttonsMap[e.button.button];
			_buttons[button] = true;
			fillPosition(e, _position);
			MouseParams params(button, _position);
			sendMessage(MessageID::MOUSE_DOWN, params);
		}
		else if(e.type == SDL_MOUSEBUTTONUP)
		{
			MouseButton::Enum button = _buttonsMap[e.button.button];
			_buttons[button] = false;
			fillPosition(e, _position);
			MouseParams params(button, _position);
			sendMessage(MessageID::MOUSE_UP, params);
		}
		else if(e.type == SDL_MOUSEMOTION)
		{
			fillPosition(e, _position);
			MouseParams params(MouseButton::NONE, _position);
			sendMessage(MessageID::MOUSE_MOVE, params);
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			MouseParams params(MouseButton::NONE, Vector(0.0f, e.wheel.y));
			sendMessage(MessageID::MOUSE_SCROLL, params);
		}
	}
}
