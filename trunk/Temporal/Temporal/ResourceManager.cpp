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
		for(tinyxml2::XMLElement* spritesheetElement = document.FirstChildElement("sprite-sheet"); spritesheetElement != NULL; spritesheetElement = spritesheetElement->NextSiblingElement())
		{
			XmlDeserializer spritesheetDeserializer(spritesheetElement);
			SpriteSheet* spritesheet = new SpriteSheet();
			spritesheet->serialize(spritesheetDeserializer);
			for(tinyxml2::XMLElement* spritegroupElement = spritesheetElement->FirstChildElement("sprite-group"); spritegroupElement != NULL; spritegroupElement = spritegroupElement->NextSiblingElement())
			{
				XmlDeserializer spritegroupDeserializer(spritegroupElement);
				SpriteGroup* spritegroup = new SpriteGroup();
				spritegroup->serialize(spritegroupDeserializer);
				spritesheet->add(spritegroup);
				for(tinyxml2::XMLElement* spriteElement = spritegroupElement->FirstChildElement("sprite"); spriteElement != NULL; spriteElement = spriteElement->NextSiblingElement())
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

	void ResourceManager::initAnimationSets()
	{
		tinyxml2::XMLDocument document;
		document.LoadFile("animations.xml");
		for(tinyxml2::XMLElement* animationSetElement = document.FirstChildElement("animation-set"); animationSetElement != NULL; animationSetElement = animationSetElement->NextSiblingElement())
		{
			XmlDeserializer animationSetDeserializer(animationSetElement);
			AnimationSet* animationSet = new AnimationSet();
			animationSet->serialize(animationSetDeserializer);
			for(tinyxml2::XMLElement* animationElement = animationSetElement->FirstChildElement("animation"); animationElement != NULL; animationElement = animationElement->NextSiblingElement())
			{
				XmlDeserializer animationDeserializer(animationElement);
				Animation* animation = new Animation();
				animation->serialize(animationDeserializer);
				for(tinyxml2::XMLElement* sampleSetElement = animationElement->FirstChildElement("sample-set"); sampleSetElement != NULL; sampleSetElement = sampleSetElement->NextSiblingElement())
				{
					XmlDeserializer sampleSetDeserializer(sampleSetElement);
					SampleSet* sampleSet = new SampleSet();
					sampleSet->serialize(sampleSetDeserializer);
					for(tinyxml2::XMLElement* sampleElement = sampleSetElement->FirstChildElement("sample"); sampleElement != NULL; sampleElement = sampleElement->NextSiblingElement())
					{
						XmlDeserializer sampleDeserializer(sampleElement);
						Sample* sample = new Sample();
						sample->serialize(sampleDeserializer);
						sampleSet->add(sample);
					}
					animation->add(sampleSet);
				}
				animationSet->add(animation);
			}
			_animationSets[animationSet->getId()] = animationSet;
		}
	}

	void ResourceManager::dispose()
	{
		for(SpriteSheetIterator i = _spritesheets.begin(); i != _spritesheets.end(); ++i)
			delete i->second;
		for(AnimationSetIterator i = _animationSets.begin(); i != _animationSets.end(); ++i)
			delete i->second;
	}
}