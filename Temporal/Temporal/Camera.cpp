#include "Camera.h"
#include "Graphics.h"
#include "EntitySystem.h"
#include "Grid.h"
#include "GameEnums.h"
#include "MessageUtils.h"
#include "LayersManager.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	Camera::Camera(LayersManager* manager) : Layer(manager), _trackedEntityId(PLAYER_ENTITY), _window(0.0f, 50.0f), _time(1.0f), _scale(1.0f)
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		_center = cameraSize / 2.0f;
		_previousCenter = Vector::Zero;
		_targetCenter = _center;
	}

	void Camera::setTrackedEntityId(Hash trackedEntityId)
	{
		void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(_trackedEntityId, Message(MessageID::GET_POSITION));
		_previousCenter = getVectorParam(result);
		_time = 0.0f;
		_trackedEntityId = trackedEntityId;
		
	}

	void Camera::draw(float framePeriod)
	{
		if(_trackedEntityId != Hash::INVALID)
		{
			void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(_trackedEntityId, Message(MessageID::GET_POSITION));
			Vector trackedEntityPosition = getVectorParam(result);
			if (_trackedEntityId == PLAYER_ENTITY)
			{
				_targetCenter.setX(trackedEntityPosition.getX());
				handleWindow(trackedEntityPosition);
			}
			else
			{
				_targetCenter = trackedEntityPosition;
			}
			if (_time >= 1.0f)
			{
				_center = _targetCenter;
			}
			else
			{
				_time += framePeriod * 2.0f;
				if (_time > 1.0f)
					_time = 1.0f;
				_center = _previousCenter + _time * (_targetCenter - _previousCenter);
			}
			
			clamp();
		}
		
		Vector translation = -getUnscaledBottomLeft();
		Graphics::get().getMatrixStack().top().translate(translation);
		Graphics::get().getMatrixStack().top().scale(Vector(_scale, _scale));
	}

	void Camera::handleWindow(const Vector& trackedEntityPosition)
	{
		Vector delta = trackedEntityPosition - _targetCenter;
		if (fabsf(delta.getY()) > _window.getY())
		{
			if (delta.getY() > 0.0f)
			{
				_targetCenter.setY(_targetCenter.getY() + delta.getY() - _window.getY());
			}
			else
			{
				_targetCenter.setY(_targetCenter.getY() + delta.getY() + _window.getY());
			}
		}
	}

	Vector Camera::getUnscaledBottomLeft() const
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraWidth = cameraSize.getX();
		float cameraHeight = cameraSize.getY();
		float cameraLeftPosition = _center.getX() - cameraWidth / 2.0f;
		float cameraBottomPosition = _center.getY() - cameraHeight / 2.0f;
		return Vector(cameraLeftPosition, cameraBottomPosition);
	}

	Vector Camera::getBottomLeft() const
	{
		return getUnscaledBottomLeft() / _scale;
	};

	Vector Camera::getOffsetPosition(const Vector& vector) const
	{
		return getBottomLeft() + vector / _scale;
	}

	Vector Camera::getParallaxPosition(float parallaxScale) const
	{
		return getUnscaledBottomLeft() * parallaxScale / _scale;
	}

	void Camera::clamp()
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraRadiusX = cameraSize.getX() / 2.0f / _scale;
		float cameraRadiusY = cameraSize.getY() / 2.0f / _scale;
		const Vector& levelSize = getManager().getGameState().getGrid().getWorldSize();
		float levelWidth = levelSize.getX();
		float levelHeight = levelSize.getY();

		

		Vector center = _center / _scale;

		if (center.getX() < cameraRadiusX)
		{
			_center.setX(cameraRadiusX * _scale);
		}
			
		if (center.getY() < cameraRadiusY)
		{
			_center.setY(cameraRadiusY * _scale);
		}
			
		if (center.getX() + cameraRadiusX > levelWidth)
		{
			_center.setX((levelWidth - cameraRadiusX) * _scale);
		}

		if (center.getY() + cameraRadiusY > levelHeight)
		{
			_center.setY((levelHeight - cameraRadiusY) * _scale);
		}
	}

	void Camera::scale(float scale)
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		const Vector& levelSize = getManager().getGameState().getGrid().getWorldSize();
		float levelWidth = levelSize.getX();
		float levelHeight = levelSize.getY();
		float newScale = _scale * scale;

		if (cameraSize.getX() / newScale > levelWidth)
		{
			newScale = cameraSize.getX() / levelWidth;
		}

		if (cameraSize.getY() / newScale > levelHeight)
		{
			newScale = cameraSize.getY() / levelHeight;
		}

		_scale = newScale;

		clamp();
	}
}
