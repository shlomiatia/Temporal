#pragma once

#include <Temporal\Base\Vector.h>

namespace Temporal
{
	class Texture
	{
	public:
		static const Texture* const load(const char* path);
		~Texture();

		unsigned int getID(void) const { return _id; }
		const Vector& getSize(void) const { return _size; }
	private:
		const unsigned int _id;
		const Vector _size;

		Texture(unsigned int id, const Vector& size) : _id(id), _size(size) {}

		Texture(const Texture&);
		Texture& operator=(const Texture&);
	};
}