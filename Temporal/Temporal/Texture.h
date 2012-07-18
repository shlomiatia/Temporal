#ifndef TEXTURE_H
#define TEXTURE_H

#include "NumericPair.h"

namespace Temporal
{
	class Texture
	{
	public:
		static const Texture* load(const char* path);
		~Texture();

		unsigned int getID() const { return _id; }
		const Size& getSize() const { return _size; }
	private:
		const unsigned int _id;
		const Size _size;

		Texture(unsigned int id, const Size& size) : _id(id), _size(size) {}

		Texture(const Texture&);
		Texture& operator=(const Texture&);
	};
}
#endif