#include "ViewManager.h"
#include "Graphics.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Game\EntitiesManager.h>

namespace Temporal
{
	void ViewManager::init(const Vector& resolution, float logicalHeight, bool fullScreen)
	{
		_cameraSize.setHeight(logicalHeight);
		setVideoMode(resolution, fullScreen);
	}

	void ViewManager::setVideoMode(const Vector& resolution, bool fullScreen)
	{
		float logicalWidth = _cameraSize.getHeight() * resolution.getWidth() / resolution.getHeight();
		_cameraSize.setWidth(logicalWidth);
		Graphics::get().init(resolution, _cameraSize, fullScreen);
	}

	void ViewManager::update(void) const
	{
		// TODO: Who're you chasing ENTITIES
		const Vector& position = *(Vector*)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));
		float cameraWidth = _cameraSize.getWidth();
		float cameraHeight = _cameraSize.getHeight();
		float levelWidth = _levelBounds.getWidth();
		float levelHeight = _levelBounds.getHeight();
		float cameraLeftPosition = position.getX() -  cameraWidth/ 2.0f;
		cameraLeftPosition = cameraLeftPosition + cameraWidth < levelWidth ? cameraLeftPosition : -cameraWidth;
		cameraLeftPosition = cameraLeftPosition < 0.0f ? 0.0f : cameraLeftPosition;
		float cameraBottomPosition = position.getY() - cameraHeight / 2.0f;
		cameraBottomPosition = cameraBottomPosition + cameraHeight < levelHeight ? cameraBottomPosition :  levelHeight - cameraHeight;
		cameraBottomPosition = cameraBottomPosition < 0.0f ? 0.0f : cameraBottomPosition;

		Vector cameraBottomLeftPosition(-cameraLeftPosition, -cameraBottomPosition);
		Graphics::get().setTranslation(cameraBottomLeftPosition);
	}
}
