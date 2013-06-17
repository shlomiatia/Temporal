#include "Camera.h"
#include "Graphics.h"
#include "EntitySystem.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Camera::draw()
	{
		void* result = _manager->getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION));
		const Vector& playerPosition = *static_cast<Vector*>(result);
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraWidth = cameraSize.getX();
		float cameraHeight = cameraSize.getY();
		float levelWidth = _levelSize.getX();
		float levelHeight = _levelSize.getY();
		float cameraLeftPosition = playerPosition.getX() -  cameraWidth/ 2.0f;
		cameraLeftPosition = cameraLeftPosition + cameraWidth < levelWidth ? cameraLeftPosition : (levelWidth-cameraWidth);
		cameraLeftPosition = cameraLeftPosition < 0.0f ? 0.0f : cameraLeftPosition;
		float cameraBottomPosition = playerPosition.getY() - cameraHeight / 2.0f;
		cameraBottomPosition = cameraBottomPosition + cameraHeight < levelHeight ? cameraBottomPosition :  (levelHeight - cameraHeight);
		cameraBottomPosition = cameraBottomPosition < 0.0f ? 0.0f : cameraBottomPosition;
		Vector cameraBottomLeft = Vector(cameraLeftPosition, cameraBottomPosition);
		Vector translation = -cameraBottomLeft;
		Graphics::get().translate(translation);
	}
}
