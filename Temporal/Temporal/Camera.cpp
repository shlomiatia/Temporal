#include "Camera.h"
#include "Graphics.h"
#include "EntitySystem.h"
#include "Grid.h"
#include "GameEnums.h"
#include "MessageUtils.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	Camera::Camera(LayersManager* manager) : Layer(manager), _trackedEntityId(PLAYER_ENTITY), _window(0.0f, 50.0f), _t(0.0f)
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		_center = cameraSize / 2.0f;
		_previousCenter = Vector::Zero;
		_targetCenter = _center;
		_t = 1.0f;
		
	}

	void Camera::setTrackedEntityId(Hash trackedEntityId)
	{
		void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(_trackedEntityId, Message(MessageID::GET_POSITION));
		_previousCenter = getVectorParam(result);
		_t = 0.0f;
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
			if (_t >= 1.0f)
			{
				_center = _targetCenter;
			}
			else
			{
				_t += framePeriod * 2.0f;
				if (_t > 1.0f)
					_t = 1.0f;
				_center = _previousCenter + _t * (_targetCenter - _previousCenter);
			}
			
			clamp();
		}
		
		Vector translation = -getBottomLeft();
		Graphics::get().getMatrixStack().top().translate(translation);
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

	void Camera::drawDebug()
	{
		
	}

	Vector Camera::getBottomLeft() const
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraWidth = cameraSize.getX();
		float cameraHeight = cameraSize.getY();
		float cameraLeftPosition = _center.getX() - cameraWidth / 2.0f;
		float cameraBottomPosition = _center.getY() - cameraHeight / 2.0f;
		return Vector(cameraLeftPosition, cameraBottomPosition);
	}

	void Camera::clamp()
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraRadiusX = cameraSize.getX() / 2.0f;
		float cameraRadiusY = cameraSize.getY() / 2.0f;
		const Vector& levelSize = getManager().getGameState().getGrid().getWorldSize();
		float levelWidth = levelSize.getX();
		float levelHeight = levelSize.getY();
		if (_center.getX() < cameraRadiusX)
			_center.setX(cameraRadiusX);
		if (_center.getY() < cameraRadiusY)
			_center.setY(cameraRadiusY);
		if (_center.getX() + cameraRadiusX > levelWidth)
			_center.setX(levelWidth - cameraRadiusX);
		if (_center.getY() + cameraRadiusY > levelHeight)
			_center.setY(levelHeight - cameraRadiusY);
	}
}
