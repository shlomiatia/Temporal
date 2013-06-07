#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Animation.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include <ftgl/ftgl.h>

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

	FTFont* ResourceManager::getFont(const char* name, unsigned int size)
	{
		std::ostringstream stream;
		stream << name << size;
		Hash id(stream.str().c_str());

		FontIterator result = _fonts.find(id);
        if(result != _fonts.end())
        {
            return result->second;
        }

		FTFont* font = new FTTextureFont(name);

        if(!font->FaceSize(size))
        {
            abort();
        }

        _fonts[id] = font;

        return font;
	}

	void ResourceManager::dispose()
	{
		for(SpriteSheetIterator i = _spritesheets.begin(); i != _spritesheets.end(); ++i)
			delete i->second;
		for(AnimationSetIterator i = _animationSets.begin(); i != _animationSets.end(); ++i)
			delete i->second;
		for(FontIterator i = _fonts.begin(); i != _fonts.end(); ++i)
			delete i->second;
	}
}