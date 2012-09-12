#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Animation.h"
#include "Serialization.h"
#include "SerializationAccess.h"


namespace Temporal
{
	void ResourceManager::init()
	{
		initSpritesheets();
		initAnimationSets();
	}

	void ResourceManager::initSpritesheets()
	{
		XmlDeserializer deserializer("spritesheets.xml");
		deserializer.serialize("sprite-sheet", _spritesheets);
	}

	void ResourceManager::initAnimationSets()
	{
		XmlDeserializer deserializer("animations.xml");
		deserializer.serialize("animation-set", _animationSets);
	}

	void ResourceManager::dispose()
	{
		for(SpriteSheetIterator i = _spritesheets.begin(); i != _spritesheets.end(); ++i)
			delete i->second;
		for(AnimationSetIterator i = _animationSets.begin(); i != _animationSets.end(); ++i)
			delete i->second;
	}
}