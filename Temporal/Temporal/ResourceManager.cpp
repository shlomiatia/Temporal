#include "ResourceManager.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Animation.h"
#include "Layer.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "GameState.h"
#include "EntitySystem.h"
#include "Log.h"
#include <ftgl/ftgl.h>
#include <windows.h>
#include <SDL_syswm.h>

namespace Temporal
{
	static HDC hdc;
	static HGLRC loaderContext;

	int runCallback(void* param)
	{
		IOThread& ioThread = *static_cast<IOThread*>(param);
		ioThread.run();
		return 0;
	}

	void IOThread::run()
	{
		wglMakeCurrent(hdc, loaderContext);
		while(_isRunning)
		{
			_semaphore.wait();
			if(!_isRunning)
				break;
			void* result = _job->load();
			_job->loaded(result);
			_job = 0;
		}
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(loaderContext);
	}

	void IOThread::setJob(IOJob* job)
	{
		_job = job;
		_semaphore.notify();
	}

	void IOThread::init()
	{
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if(SDL_GetWMInfo(&wmInfo) < 0)
		{
			abort();
		}
		HWND hwnd = wmInfo.window;
		hdc = GetDC(hwnd);
		HGLRC mainContext = wglGetCurrentContext();
		loaderContext = wglCreateContext(hdc);
		wglShareLists(loaderContext, mainContext);

		_isRunning = true;
		_thread.start(runCallback, this);
	}

	void IOThread::dispose()
	{
		_isRunning = false;
		_semaphore.notify();
	}

	GameState* IOAPI::loadGameState(const char* gameStateFile)
	{
		XmlDeserializer deserializer(gameStateFile);
		GameState* state = new GameState();
		deserializer.serialize("game-state", *state);
		state->init();
		return state;
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

	const std::shared_ptr<SpriteSheet> ResourceManager::getTexture(const char* file)
	{
		Hash id = Hash(file);

		SpriteSheetIterator result = _spritesheets.find(id);
        if(result != _spritesheets.end())
        {
            return result->second;
        }

		SpriteSheet* spriteSheet = new SpriteSheet(const_cast<char*>(file));
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

	void ResourceManager::collectGarbage()
	{
		for(SpriteSheetIterator i = _spritesheets.begin(); i != _spritesheets.end(); ++i)
		{
			if(i->second.unique())
				i = _spritesheets.erase(i);
		}

		for(AnimationSetIterator i = _animationSets.begin(); i != _animationSets.end(); ++i)
		{
			if(i->second.unique())
				i = _animationSets.erase(i);
		}

		for(FontIterator i = _fonts.begin(); i != _fonts.end(); ++i)
		{
			if(i->second.unique())
				i = _fonts.erase(i);
		}
	}

	void ResourceManager::init()
	{
	}

	void ResourceManager::dispose()
	{
	}
}