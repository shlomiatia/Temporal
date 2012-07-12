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
		const Sprite& sprite = spritesheet.get(getSpriteGroupID()).get(getSpriteID());
		Graphics::get().beginTranslate(getTranslation());
		Vector offset = Vector(-sprite.getOffset().getVx()  * orientation * spritesheetOrientation, -sprite.getOffset().getVy());
		Graphics::get().draw(spritesheet.getTexture(), sprite.getBounds(), offset, mirrored);
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			(*i)->draw(spritesheet, orientation);
		}
		Graphics::get().endTranslate();
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