#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "Vector.h"
#include "Shapes.h"
#include "Color.h"
#include <vector>

namespace Temporal
{
	class Texture;

	class SpriteBatchItem
	{
	public:
		SpriteBatchItem() : texture(0), rotation(0.0f), scale(0.0f), color(Color::White) {}

		const Texture* texture;
		Vector objectTranslation;
		float rotation;
		Vector rotationTranslation;
		float scale;
		Vector size;
		Color color;
		AABB texturePart;

		unsigned int getTextureId() const;
		const Vector& getSize() const;
	};

	namespace SpriteBatchMode
	{
		enum Enum
		{
			LINES,
			TRIANGLES
		};
	}

	typedef std::vector<SpriteBatchItem> SpriteBatchItemCollection;
	typedef SpriteBatchItemCollection::const_iterator SpriteBatchItemIterator;

	class SpriteBatch
	{
	public:

		SpriteBatch() : _mode(SpriteBatchMode::TRIANGLES), _size(0), _vertices(0), _vao(0), _vbo(0), _ibo(0), _lastTexture(0), 
			_items(INITLAL_MAX_SPRITES), _coordinateAttribute(0), _textureCoordinateAttribute(0), _colorAttribute(0), _textureUniform(0),
			_transformUniform(0), _typeUniform(0) {}
		~SpriteBatch();

		void init();
		void begin();
		void add(const Texture* texture, const AABB& texturePart, const Vector& objectTranslation, float rotation = 0.0f, const Vector& rotationTranslation = Vector::Zero, 
			float scale = 1.0f, const Vector& size = Vector::Zero, const Color color = Color::White);
		void add(const Vector& objectTranslation, const Vector& size, float rotation = 0.0f, const Vector& rotationTranslation = Vector::Zero, 
			float scale = 1.0f, const Color color = Color::White);
		void end();

		void setMode(SpriteBatchMode::Enum mode) { _mode = mode; }
	
	private:
		static const int INITLAL_MAX_SPRITES = 256;

		SpriteBatchMode::Enum _mode;
		
		std::vector<SpriteBatchItem> _items;
		int _size;
		float* _vertices;
		unsigned int _vao, _vbo, _ibo;
		
		unsigned int _lastTexture;

		int _coordinateAttribute, _textureCoordinateAttribute, _colorAttribute;
		int _textureUniform, _transformUniform, _typeUniform;

		int getCoordinateAttribute() const { return _coordinateAttribute; }
		int getTextureCoordinateAttribute() const { return _textureCoordinateAttribute; }
		int getColorAttribute() const { return _colorAttribute; }
		int getTextureUniform() const { return _textureUniform; }
		int getTransformUniform() const { return _transformUniform; }

		void addIBOLines(unsigned short* elements, int i);
		void addIBOTriangles(unsigned short* elements, int i);

		void expand(const int maxSprites);
		void flush(unsigned int texture, int size);

		int vboFloats() { return 4 * /* quad */ (2 + /* position(x,y)*/ 2 /* texture(x,y) */ + 4 /*color(r,g,b,a) */); }
		int iboFloats() { return _mode == SpriteBatchMode::TRIANGLES ? 6 : 8;}
		int vboVertices(int size) {	return  size * vboFloats();	}
		int iboVertices(int size) {	return size * iboFloats(); }
		int vboBytes(int size) { return vboVertices(size) * sizeof(float); }
		int iboBytes(int size) { return iboVertices(size) * sizeof(unsigned short); }
	
	};

}

#endif