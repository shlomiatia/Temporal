#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector.h"

namespace Temporal
{
	class Texture
	{
	public:
		static const Texture* load(const char* path);
		static const Texture* load(const Vector& size, bool greyscale = false);
		~Texture();

		unsigned int getID() const { return _id; }
		const Vector& getSize() const { return _size; }
	private:
		const unsigned int _id;
		const Vector _size;

		Texture(unsigned int id, const Vector& size) : _id(id), _size(size) {}

		Texture(const Texture&);
		Texture& operator=(const Texture&);
	};
}
#endif