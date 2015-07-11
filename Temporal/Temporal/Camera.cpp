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

	Camera::Camera(LayersManager* manager, bool followPlayer) : Layer(manager), _followPlayer(followPlayer), _foundPlayer(false), _center(Vector::Zero), _targetCenter(Vector::Zero), _activeCameraControl(Hash::INVALID)
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
			const Vector& playerPosition = getVectorParam(result);
			setCenter(playerPosition);
			if (!_foundPlayer)
			{
				_foundPlayer = true;
				_center = _targetCenter;
			}
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

	const Hash CameraControl::TYPE("camera-control");
	
	void CameraControl::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::UPDATE)
		{
			float alpha = getEntity().getManager().getGameState().getLayersManager().getDebugLater().isCameraControl() ? 1.0f : 0.0f;
			raiseMessage(Message(MessageID::SET_ALPHA, &alpha));

			void* result = getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION));
			const Vector& playerPosition = getVectorParam(result);
			Vector position = getPosition(*this);
			const Vector& cameraSize = Graphics::get().getLogicalView();
				
			bool inY = fabsf(position.getY() - playerPosition.getY()) < cameraSize.getY() / 2.0f;
			float diffX = cameraSize.getX() / 2.0f + cameraSize.getX() / 5.0f - fabsf(position.getX() - playerPosition.getX());
			float interpolationX = diffX / (cameraSize.getX() / 4.0f);
			Camera& camera = getEntity().getManager().getGameState().getLayersManager().getCamera();
			if (!inY || interpolationX < 0.0f)
			{
				if (camera.getActiveCameraControl() == getEntity().getId())
				{
					camera.setFollowPlayer(true);
					camera.setActiveCameraControl(Hash::INVALID);
				}
			}
			else if (camera.getActiveCameraControl() == Hash::INVALID ||
					 camera.getActiveCameraControl() == getEntity().getId() ||
					(camera.getActiveCameraControl() != getEntity().getId() && camera.getCenter().getX() < position.getX()))
			{
				if (interpolationX < 1.0f)
				{
					float newX = easeInOutBezier(interpolationX, playerPosition.getX(), position.getX());
					position.setX(newX);
				}
				
				camera.setFollowPlayer(false);
				camera.setCenter(position);
				camera.setActiveCameraControl(getEntity().getId());
			}
			
		}
	}
}
