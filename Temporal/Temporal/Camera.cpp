#include "Camera.h"
#include "Graphics.h"
#include "EntitySystem.h"
#include "Grid.h"

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Camera::draw()
	{
		if(_followPlayer)
		{
			void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION));
			const Vector& playerPosition = *static_cast<Vector*>(result);
			setCenter(playerPosition);
		}
		
		Vector translation = -_bottomLeft;
		Graphics::get().getMatrixStack().translate(translation);
	}

	void Camera::setCenter(const Vector& position)
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraWidth = cameraSize.getX();
		float cameraHeight = cameraSize.getY();
		float cameraLeftPosition = position.getX() -  cameraWidth/ 2.0f;
		float cameraBottomPosition = position.getY() - cameraHeight / 2.0f;
		Vector bottomLeft(cameraLeftPosition, cameraBottomPosition);
		setBottomLeft(bottomLeft);
	}

	void Camera::setBottomLeft(const Vector& bottomLeft)
	{
		const Vector& cameraSize = Graphics::get().getLogicalView();
		float cameraWidth = cameraSize.getX();
		float cameraHeight = cameraSize.getY();
		const Vector& levelSize = getManager().getGameState().getGrid().getWorldSize();
		float levelWidth = levelSize.getX();
		float levelHeight = levelSize.getY();
		float cameraLeftPosition = bottomLeft.getX() + cameraWidth < levelWidth ? bottomLeft.getX() : (levelWidth - cameraWidth);
		cameraLeftPosition = cameraLeftPosition < 0.0f ? 0.0f : cameraLeftPosition;
		float cameraBottomPosition = bottomLeft.getY() + cameraHeight < levelHeight ? bottomLeft.getY():  (levelHeight - cameraHeight);
		cameraBottomPosition = cameraBottomPosition < 0.0f ? 0.0f : cameraBottomPosition;
		_bottomLeft = Vector(cameraLeftPosition, cameraBottomPosition);
	}
}
