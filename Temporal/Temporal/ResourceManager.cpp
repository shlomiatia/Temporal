#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "tinyxml2.h"
#include "Serialization.h"

namespace Temporal
{
	void ResourceManager::init()
	{
		tinyxml2::XMLDocument document;
		document.LoadFile("spritesheets.xml");
		tinyxml2::XMLElement* element = document.FirstChildElement();
		for(tinyxml2::XMLElement* spritesheetElement = document.FirstChildElement("sprite-sheet"); spritesheetElement != NULL; spritesheetElement = spritesheetElement->NextSiblingElement())
		{
			XmlDeserializer spritesheetDeserializer(spritesheetElement);
			SpriteSheet* spritesheet = new SpriteSheet();
			spritesheet->serialize(spritesheetDeserializer);
			for(tinyxml2::XMLElement* spritegroupElement = document.FirstChildElement("sprite-group"); spritegroupElement != NULL; spritegroupElement = spritegroupElement->NextSiblingElement())
			{
				XmlDeserializer spritegroupDeserializer(spritegroupElement);
				SpriteGroup* spritegroup = new SpriteGroup();
				spritegroup->serialize(spritegroupDeserializer);
				spritesheet->add(spritegroup);
				for(tinyxml2::XMLElement* spriteElement = document.FirstChildElement("sprite"); spriteElement != NULL; spriteElement = spriteElement->NextSiblingElement())
				{
					XmlDeserializer spriteDeserializer(spriteElement);
					Sprite* sprite = new Sprite();
					sprite->serialize(spriteDeserializer);
					spritegroup->add(sprite);
				}
			}
			spritesheet->init();
			_spritesheets[spritesheet->getId()] = spritesheet;
		}
	}

	const SpriteSheet* ResourceManager::getSpritesheet(Hash id)
	{
		return _spritesheets.at(id);
	}

	void ResourceManager::dispose()
	{
		for(SpriteSheetIterator i = _spritesheets.begin(); i != _spritesheets.end(); ++i)
			delete i->second;
	}
}