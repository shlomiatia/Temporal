#include "CameraControl.h"
#include "Camera.h"
#include "ActionController.h"
#include "Graphics.h"
#include "LayersManager.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void CameraControl::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::UPDATE)
		{
			Camera& camera = getEntity().getManager().getGameState().getLayersManager().getCamera();
			Hash trackedEntityId = camera.getTrackedEntityId();
			const Vector& cameraCurrentTrackedEntityPosition = getVectorParam(getEntity().getManager().sendMessageToEntity(trackedEntityId, Message(MessageID::GET_POSITION)));
			const Vector& playerPosition = getVectorParam(getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION)));
			const OBB& playerShape = getOBBParam(getEntity().getManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_SHAPE)));
			Vector position = getPosition(*this);

			Vector positionToPlayerPositionDelta = (position - playerPosition).absolute();

			if (trackedEntityId != PLAYER_ENTITY &&
				trackedEntityId != getEntity().getId() &&
				positionToPlayerPositionDelta.getLength() > (cameraCurrentTrackedEntityPosition - playerPosition).getLength())
			{
				return;
			}

			Vector positionToPlayerShapeDelta = positionToPlayerPositionDelta + playerShape.getRadius();
			const Vector& cameraRadius = Graphics::get().getLogicalView() / 2.0f;

			if (positionToPlayerShapeDelta.getX() <= cameraRadius.getX() && positionToPlayerShapeDelta.getY() <= cameraRadius.getY())
			{
				if (camera.getTrackedEntityId() != getEntity().getId())
				{
					camera.setTrackedEntityId(getEntity().getId());
				}
			}
			else if (camera.getTrackedEntityId() == getEntity().getId())
			{
				camera.setTrackedEntityId(PLAYER_ENTITY);
			}
		}
	}
}
