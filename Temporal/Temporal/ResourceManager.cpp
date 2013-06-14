#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Animation.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "GameState.h"
#include "EntitySystem.h"
#include <ftgl/ftgl.h>

namespace Temporal
{
	int runCallback(void* param)
	{
		ResourceManager& resourceManager = *static_cast<ResourceManager*>(param);
		resourceManager.run();
		return 0;
	}

	void ResourceManager::run()
	{
		while(_isRunning)
		{
			_semaphore.wait();
			if(!_isRunning)
				break;
			GameState* state = loadGameState(_gameStateFile);
			GameStateManager::get().gameStateReady(state);
		}
	}

	void ResourceManager::init()
	{
		_isRunning = true;
		_thread.start(runCallback, this);
	}

	void ResourceManager::dispose()
	{
		_isRunning = false;
		_semaphore.notify();
	}

	GameState* ResourceManager::loadGameState(const char* gameStateFile)
	{
		XmlDeserializer deserializer(gameStateFile);
		GameState* state = new GameState();
		deserializer.serialize("game-state", *state);
		state->init();
		return state;
	}

	void ResourceManager::queueLoadGameState(const char* gameStateFile)
	{
		_gameStateFile = gameStateFile;
		_semaphore.notify();
	}

	const std::shared_ptr<SpriteSheet> ResourceManager::getSpritesheet(const char* file)
	{
		Hash id = Hash(file);

		SpriteSheetIterator result = _spritesheets.find(id);
        if(result != _spritesheets.end())
        {
            return result->second;
        }

		XmlDeserializer deserializer(file);
		SpriteSheet* spriteSheet = new SpriteSheet();
		deserializer.serialize("sprite-sheet", *spriteSheet);
		spriteSheet->init();
		std::shared_ptr<SpriteSheet> spriteSheetP(spriteSheet);
		_spritesheets[id] = spriteSheetP;

		return spriteSheetP;
	}

	const std::shared_ptr<AnimationSet> ResourceManager::getAnimationSet(const char* file)
	{
		Hash id = Hash(file);

		AnimationSetIterator result = _animationSets.find(id);
        if(result != _animationSets.end())
        {
            return result->second;
        }

		XmlDeserializer deserializer(file);
		AnimationSet* animationSet = new AnimationSet();
		deserializer.serialize("animation-set", *animationSet);
		animationSet->init();
		std::shared_ptr<AnimationSet> animationSetP(animationSet);
		_animationSets[id] = animationSetP;

		return animationSetP;
	}

	const std::shared_ptr<FTFont> ResourceManager::getFont(const char* name, unsigned int size)
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
		std::shared_ptr<FTFont> fontP(font);
        _fonts[id] = fontP;

        return fontP;
	}
}