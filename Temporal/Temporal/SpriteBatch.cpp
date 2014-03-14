#include "SpriteBatch.h"
#include "Graphics.h"
#include "Texture.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace Temporal
{
	inline bool compare(const SpriteBatchItem& a, const SpriteBatchItem& b)
	{
		return a.getTextureId() < b.getTextureId();
	}

	unsigned int SpriteBatchItem::getTextureId() const 
	{
		return texture == 0 ? 0 : texture->getID(); 
	}

	const Vector& SpriteBatchItem::getSize() const
	{
		return size == Vector::Zero ? size : texture->getSize();
	}

	void SpriteBatch::init() 
	{
		ShaderProgram& program = Graphics::get().getShaderProgram();
		_coordinateAttribute = program.getAttribute("a_coordinate");
		_textureCoordinateAttribute = program.getAttribute("a_textureCoordinate");
		_colorAttribute = program.getAttribute("a_color");
	
		_transformUniform = program.getUniform("u_transform");
		_textureUniform = program.getUniform("u_texture");
	
		_typeUniform = program.getUniform("u_type");

		program.setUniform(_typeUniform, 0);

		glGenBuffers(1, &_vbo);
		glGenBuffers(1, &_ibo);

		expand(INITLAL_MAX_SPRITES);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glEnableVertexAttribArray(getCoordinateAttribute());
	
		glVertexAttribPointer(
			getCoordinateAttribute(), // attribute
			2,                 // number of elements per vertex, here (x,y)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			vboFloats(),                 // no extra data between each position
			0                  // offset of first element
		);

		glEnableVertexAttribArray(getTextureCoordinateAttribute());
		glVertexAttribPointer(
			getTextureCoordinateAttribute(), // attribute
			2,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			vboFloats(),                 // no extra data between each position
			(void*)8			// offset of first element
		);

		glEnableVertexAttribArray(getColorAttribute());
		glVertexAttribPointer(
			getColorAttribute(), // attribute
			4,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			vboFloats(),                 // no extra data between each position
			(void*)16    // offset of first element
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

		glBindVertexArray(0);
		glDisableVertexAttribArray(getCoordinateAttribute());
		glDisableVertexAttribArray(getTextureCoordinateAttribute());
		glDisableVertexAttribArray(getColorAttribute());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	SpriteBatch::~SpriteBatch()
	{
		delete[] _vertices;
		glDeleteBuffers(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ibo);
	}

	void SpriteBatch::addIBOLines(unsigned short* elements, int i)
	{
		int k = i * 8;
		int j = i * 4;
		elements[k++] = j;
		elements[k++] = j+1;
		elements[k++] = j+1;
		elements[k++] = j+2;
		elements[k++] = j+2;
		elements[k++] = j+3;
		elements[k++] = j+3;
		elements[k++] = j;
	}

	void SpriteBatch::addIBOTriangles(unsigned short* elements, int i)
	{
		int k = i * 6;
		int j = i * 4;
		elements[k++] = j;
		elements[k++] = j+1;
		elements[k++] = j+2;
		elements[k++] = j+2;
		elements[k++] = j+3;
		elements[k++] = j;
	}

	void SpriteBatch::expand(const int maxSprites)
	{
		if(_vertices)
			delete[] _vertices;
		_vertices = new GLfloat[vboVertices(maxSprites)];
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, vboBytes(maxSprites), _vertices, GL_DYNAMIC_DRAW);
		
		GLushort* elements= new GLushort[iboVertices(maxSprites)];
		for(int i = 0; i < maxSprites; ++i)
		{
			_mode == SpriteBatchMode::TRIANGLES ? addIBOTriangles(elements, i) : addIBOLines(elements, i);
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboBytes(maxSprites), elements, GL_STATIC_DRAW);
		delete elements;
	}

	void SpriteBatch::begin()
	{
		_lastTexture = 0;
		_size = 0;
	}

	void SpriteBatch::add(const Texture* texture, const AABB& texturePart, const Vector& objectTranslation, float rotation, 
		const Vector& rotationTranslation, float scale, const Vector& size, const Color color)
	{
		if(_size == _items.size())
		{
			_items.resize(_items.size() * 2, SpriteBatchItem());
			expand(_items.size());
		
		}
		SpriteBatchItem& item = _items[_size];
		item.texture = texture;
		item.texturePart = texturePart;
		item.objectTranslation = objectTranslation;
		item.rotation = rotation;
		item.rotationTranslation = rotationTranslation;
		item.scale = scale;
		item.size = size;
		item.color = color;
		++_size;
	}

	void SpriteBatch::add(const Vector& objectTranslation, const Vector& size, float rotation, const Vector& rotationTranslation, float scale, const Color color) 
	{
		add(0, AABB::Zero, objectTranslation, rotation, rotationTranslation, scale, size, color);
	}

	void SpriteBatch::end()
	{
		std::sort(_items.begin(), _items.begin() + _size, compare);

		int index = 0;
		for(SpriteBatchItemIterator itemI = _items.begin(); itemI != _items.begin() + _size; ++itemI)
		{	
			const SpriteBatchItem& item = *itemI;
			unsigned int texture = item.getTextureId();
			if(_lastTexture != texture)
			{
				flush(_lastTexture, index);
				_lastTexture = texture;
				index = 0;
			}
			
			int i = index * vboFloats();
			++index;
	
			glm::vec4 a;
			glm::vec4 b;
			glm::vec4 c;
			glm::vec4 d;
			
			Vector radius = item.getSize() / 2.0f;
			a = glm::vec4(item.objectTranslation.getX() - radius.getX(), item.objectTranslation.getY() - radius.getY(), 0.0f, 0.0f);
			b = glm::vec4(item.objectTranslation.getX() + radius.getX(), item.objectTranslation.getY() - radius.getY(), 0.0f, 0.0f);
			c = glm::vec4(item.objectTranslation.getX() + radius.getX(), item.objectTranslation.getY() + radius.getY(), 0.0f, 0.0f);
			d = glm::vec4(item.objectTranslation.getX() - radius.getX(), item.objectTranslation.getY() + radius.getY(), 0.0f, 0.0f);
			
			if(item.rotation != 0.0f)
			{
				glm::mat4 m = glm::mat4();
				// scale
				m = glm::translate(m, glm::vec3(item.rotationTranslation.getX(), item.rotationTranslation.getY(), 0.0f));
				m = glm::rotate(m, item.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
				a = m * a;
				b = m * b;
				c = m * c;
				d = m * d;
			}

			_vertices[i++] = a.x;
			_vertices[i++] = a.y;
			_vertices[i++] = item.texturePart.getLeft();
			_vertices[i++] = item.texturePart.getBottom();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
			_vertices[i++] = b.x;
			_vertices[i++] = b.y;
			_vertices[i++] = item.texturePart.getRight();
			_vertices[i++] = item.texturePart.getBottom();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
			_vertices[i++] = c.x;
			_vertices[i++] = c.y;
			_vertices[i++] = item.texturePart.getRight();
			_vertices[i++] = item.texturePart.getTop();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
			_vertices[i++] = d.x; 
			_vertices[i++] = d.y;
			_vertices[i++] = item.texturePart.getLeft();
			_vertices[i++] = item.texturePart.getTop();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
		}
		flush(_lastTexture, index);
	}

	void SpriteBatch::flush(GLuint texture, int size)
	{
		if(size == 0)
			return;
		glUseProgram(Graphics::get().getShaderProgram().getId());
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(_vao);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(getTextureUniform(), 0);
		glUniformMatrix4fv(getTransformUniform(), 1, GL_FALSE, glm::value_ptr(Graphics::get().getMatrixStack().top()));
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		
		int bytes = vboBytes(size);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, _vertices);
		glDrawElements(_mode == SpriteBatchMode::TRIANGLES ? GL_TRIANGLES : GL_LINES, iboVertices(size), GL_UNSIGNED_SHORT, 0);

		glBindVertexArray(0);	
		glUseProgram(0);
	}
}