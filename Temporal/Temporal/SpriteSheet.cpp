#include "SpriteSheet.h"
#include "Texture.h"
#include <assert.h>

namespace Temporal
{
	SpriteGroup::~SpriteGroup()
	{
		for(SpriteIterator i = _sprites.begin(); i != _sprites.end(); ++i)
			delete *i;
	}

	void SpriteGroup::add(Sprite* sprite)
	{
		_sprites.push_back(sprite);
	}

	const Sprite& SpriteGroup::get(int spriteID) const
	{
		assert(static_cast<unsigned int>(spriteID) < _sprites.size());
		return *_sprites.at(spriteID);
	}

	int SpriteGroup::getSize() const
	{
		return _sprites.size();
	}

	SpriteSheet::SpriteSheet(char* textureFile)
	{
		_textureFile = textureFile;
		init();
		SpriteGroup* spriteGroup = new SpriteGroup();
		add(spriteGroup);
		Vector radius = _texture->getSize() / 2.0f;
		Sprite* sprite = new Sprite(AABB(radius, radius));
		spriteGroup->add(sprite);
	}

	SpriteSheet::~SpriteSheet()
	{
		delete _texture;
		for(SpriteGroupIterator i = _spriteGroups.begin(); i != _spriteGroups.end(); ++i)
			delete i->second;
	}

	void SpriteSheet::add(SpriteGroup* spriteGroup)
	{
		_spriteGroups[spriteGroup->getId()] = spriteGroup;
	}

	const SpriteGroup& SpriteSheet::get(Hash spriteGroupID) const
	{
		return *_spriteGroups.at(spriteGroupID);
	}

	void SpriteSheet::init()
	{
		_texture = Texture::load(_textureFile);
	}
}