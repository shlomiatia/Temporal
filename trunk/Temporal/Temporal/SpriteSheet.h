#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "Hash.h"
#include "BaseEnums.h"
#include "NumericPair.h"
#include "AABB.h"
#include <vector>
#include <unordered_map>

namespace Temporal
{
	class Sprite
	{
	public:
		Sprite(const AABB& bounds, const Vector& offset) 
			: _bounds(bounds), _offset(offset) {}

		const AABB& getBounds() const { return _bounds; }
		const Vector& getOffset() const { return _offset; }
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
		SpriteGroup() {}
		~SpriteGroup();

		void add(const Sprite* sprite);

		const Sprite& get(int spriteID) const;
		int getSize() const;
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
		~SpriteSheet();
		void add(const Hash& id, const SpriteGroup* element);

		const Texture& getTexture() const { return *_texture; }
		Side::Enum getOrientation() const { return _orientation; }
		const SpriteGroup& get(const Hash& spriteGroupID) const;
		const Hash& getFirstSpriteGroupID() const;
	private:
		const Texture* _texture;
		const Side::Enum _orientation;
		SpriteGroupCollection _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}
#endif