#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <Temporal\Base\Hash.h>
#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\AABB.h>
#include <vector>
#include <unordered_map>

namespace Temporal
{
	class Sprite
	{
	public:
		Sprite(const AABB& bounds, const Vector& offset) 
			: _bounds(bounds), _offset(offset) {}

		const AABB& getBounds(void) const { return _bounds; }
		const Vector& getOffset(void) const { return _offset; }
	private:
		const AABB _bounds;
		const Vector _offset;

		Sprite(const Sprite&);
		Sprite& operator=(const Sprite&);
	};

	typedef std::vector<const Sprite*> SpriteCollection;
	typedef SpriteCollection::const_iterator SpriteIterator;

	class SpriteGroup
	{
	public:
		SpriteGroup(void) {}
		~SpriteGroup(void);

		void add(const Sprite* sprite);

		const Sprite& get(int spriteID) const;
		int getSize(void) const;
	private:
		SpriteCollection _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};

	class Texture;
	typedef std::unordered_map<const Hash, const SpriteGroup*> SpriteGroupCollection;
	typedef SpriteGroupCollection::const_iterator SpriteGroupIterator;

	class SpriteSheet
	{
	public:
		SpriteSheet(const Texture* texture, Side::Enum orientation) : _texture(texture), _orientation(orientation) {}
		~SpriteSheet(void);
		void add(const Hash& id, const SpriteGroup* element);

		const Texture& getTexture(void) const { return *_texture; }
		Side::Enum getOrientation(void) const { return _orientation; }
		const SpriteGroup& get(const Hash& spriteGroupID) const;
		const Hash& getFirstSpriteGroupID(void) const;
	private:
		const Texture* _texture;
		const Side::Enum _orientation;
		mutable SpriteGroupCollection _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}
#endif