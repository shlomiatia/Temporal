#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Animation.h"
#include "tinyxml2.h"
#include "Serialization.h"

namespace Temporal
{
	void ResourceManager::init()
	{
		initSpritesheets();
		initAnimationSets();
	}

	void ResourceManager::initSpritesheets()
	{
		tinyxml2::XMLDocument document;
		document.LoadFile("spritesheets.xml");
		XmlDeserializer deserializer(document.GetDocument());
		deserializer.serialize("sprite-sheet", _spritesheets);
	}

	void ResourceManager::initAnimationSets()
	{
		tinyxml2::XMLDocument document;
		document.LoadFile("animations.xml");
		XmlDeserializer deserializer(document.GetDocument());
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