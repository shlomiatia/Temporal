#include "SceneNode.h"
#include "Graphics.h"
#include "SpriteSheet.h"

namespace Temporal
{
	SceneNode::~SceneNode(void)
	{
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			delete *i;
		}
	}

	void SceneNode::draw(const SpriteSheet& spritesheet, Side::Enum orientation)
	{
		Side::Enum spritesheetOrientation = spritesheet.getOrientation();
		bool mirrored = orientation != spritesheetOrientation;
		const Sprite& sprite = spritesheet.get(getSpriteGroupId()).get(getSpriteId());
		const Vector& offset = sprite.getOffset();
		float screenLocationX = (getTranslation().getVx() - offset.getVx()) * orientation * spritesheetOrientation;
		float screenLocationY = getTranslation().getVy() - offset.getVy();
		Point screenLocation(screenLocationX, screenLocationY);
		Graphics::get().beginDraw(spritesheet.getTexture(), sprite.getBounds(), screenLocation, mirrored);
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			(*i)->draw(spritesheet, orientation);
		}
		Graphics::get().end();
	}

	SceneNode* SceneNode::get(const Hash& id)
	{
		if(_id == id)
		{
			return this;
		}
		SceneNode* result = NULL;
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			result = (*i)->get(id);
			if(result != NULL)
			{
				return result;
			}
		}
		return NULL;
	}
}