#include "Camera.h"
#include "Graphics.h"
#include "EntitySystem.h"

namespace Temporal
{
	static const Hash PLAYER = Hash("ENT_PLAYER");

	void Camera::draw()
	{
		const Vector& position = *static_cast<const Vector*>(EntitiesManager::get().sendMessageToEntity(PLAYER, Message(MessageID::GET_POSITION)));
		const Size& cameraSize = Graphics::get().getLogicalView();
		float cameraWidth = cameraSize.getWidth();
		float cameraHeight = cameraSize.getHeight();
		float levelWidth = _levelSize.getWidth();
		float levelHeight = _levelSize.getHeight();
		float cameraLeftPosition = position.getX() -  cameraWidth/ 2.0f;
		cameraLeftPosition = cameraLeftPosition + cameraWidth < levelWidth ? cameraLeftPosition : (levelWidth-cameraWidth);
		cameraLeftPosition = cameraLeftPosition < 0.0f ? 0.0f : cameraLeftPosition;
		float cameraBottomPosition = position.getY() - cameraHeight / 2.0f;
		cameraBottomPosition = cameraBottomPosition + cameraHeight < levelHeight ? cameraBottomPosition :  (levelHeight - cameraHeight);
		cameraBottomPosition = cameraBottomPosition < 0.0f ? 0.0f : cameraBottomPosition;
		Vector cameraBottomLeft = Vector(cameraLeftPosition, cameraBottomPosition);
		Vector translation = -cameraBottomLeft;
		Graphics::get().translate(translation);
	}
}
