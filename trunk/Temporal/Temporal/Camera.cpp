#include "Camera.h"
#include "Graphics.h"
#include "EntitySystem.h"
#include "Grid.h"
#include "GameEnums.h"
#include "MessageUtils.h"
#include "Sensor.h"
#include "Math.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");
	static const float MAX_CHANGE_PER_SECOND = 500.0f;

	Camera::Camera(LayersManager* manager, bool followPlayer) : Layer(manager), _followPlayer(followPlayer), _center(Vector::Zero), _targetCenter(Vector::Zero)
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		_center = cameraSize / 2.0f;
		_targetCenter = cameraSize / 2.0f;
	}

	void Camera::draw(float framePeriod)
	{
		if(_followPlayer)
		{
			void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION));
			const Vector& playerPosition = *static_cast<Vector*>(result);
			setCenter(playerPosition);
		}
		
		Vector movement = _targetCenter - _center;;
		
		float maxChange = MAX_CHANGE_PER_SECOND * framePeriod;
		float modifierX = abs(maxChange / movement.getX());
		if (modifierX < 1.0f && modifierX > 0.0f)
			movement.setX(movement.getX() * modifierX);
		float modifierY = abs(maxChange / movement.getY());
		if (modifierY < 1.0f && modifierY > 0.0f)
			movement.setY(movement.getY() * modifierY);

		_center += movement;
		
		Vector translation = -getBottomLeft();
		Graphics::get().getMatrixStack().top().translate(translation);
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
		if (_targetCenter.getX() < cameraRadiusX)
			_targetCenter.setX(cameraRadiusX);
		if (_targetCenter.getY() < cameraRadiusY)
			_targetCenter.setY(cameraRadiusY);
		if (_targetCenter.getX() + cameraRadiusX > levelWidth)
			_targetCenter.setX(levelWidth - cameraRadiusX);
		if (_targetCenter.getY() + cameraRadiusY > levelHeight)
			_targetCenter.setY(levelHeight - cameraRadiusY);
	}

	static Hash CAMERA_CONTROL_SENSOR_ID("SNS_CAMERA_CONTROL");

	CameraControl* CameraControl::_active = 0;

	void CameraControl::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::SENSOR_SENSE)
		{
			const SensorParams& params = getSensorParams(message.getParam());
			if (params.getSensorId() == CAMERA_CONTROL_SENSOR_ID)
			{
				_shouldActivate = true;
				_radius = params.getContact().getSource().getGlobalShape().getRadius();
				_activatorPosition = params.getContact().getTarget().getGlobalShape().getCenter();
			}
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			if (_shouldActivate && _active != this)
			{
				_active = this;
			}
			else if (_shouldActivate && _active == this)
			{
				Vector position = getPosition(*this);
				
				float diffX = _radius.getX() - fabsf(position.getX() - _activatorPosition.getX());
				float interpolation = diffX / (_radius.getX() / 3.0f);
				if (interpolation < 0.0f)
				{
					interpolation = 0.0f;
				}
				if (interpolation < 1.0f)
				{
					float newX = easeInOutBezier(interpolation, _activatorPosition.getX(), position.getX());
					position.setX(newX);
				}

				Camera& camera = getEntity().getManager().getGameState().getLayersManager().getCamera();
				camera.setFollowPlayer(false);
				camera.setCenter(position);
			}
			else if (!_shouldActivate && _active == this)
			{
				_active = 0;
				getEntity().getManager().getGameState().getLayersManager().getCamera().setFollowPlayer(true);
			}
			_shouldActivate = false;
		}
	}

	const Hash CameraControl::TYPE("camera-control");
}
