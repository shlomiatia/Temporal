#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\Vector.h>
#include <Temporal\Base\Rect.h>
#include <vector>

namespace Temporal
{
	class Sprite
	{
	public:
		Sprite(const Rect& bounds, const Vector& offset) 
			: _bounds(bounds), _offset(offset) {}

		const Rect& getBounds(void) const { return _bounds; }
		const Vector& getOffset(void) const { return _offset; }
	private:
		const Rect _bounds;
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
	typedef std::vector<const SpriteGroup*> SpriteGroupCollection;
	typedef SpriteGroupCollection::const_iterator SpriteGroupIterator;

	class SpriteSheet
	{
	public:
		SpriteSheet(const Texture* texture, Orientation::Enum orientation) : _texture(texture), _orientation(orientation) {}
		~SpriteSheet(void);
		void add(const SpriteGroup* element);

		const Texture& getTexture(void) const { return *_texture; }
		Orientation::Enum getOrientation(void) const { return _orientation; }
		const SpriteGroup& get(int spriteGroupID) const;
	private:
		const Texture* _texture;
		const Orientation::Enum _orientation;
		SpriteGroupCollection _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}
#endif