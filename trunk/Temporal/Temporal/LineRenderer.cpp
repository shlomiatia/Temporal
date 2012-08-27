#include "LineRenderer.h"
#include "Graphics.h"
#include "Serialization.h"
#include "BaseUtils.h"
#include "Shapes.h"
#include "Math.h"
#include "Texture.h"
#include "MessageUtils.h"
#include <SDL_opengl.h>

namespace Temporal
{
	static const VectorSerializer TARGET_SERIALIZER("LRE_SER_TAR");

	void LineRenderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLOR)
		{
			const Color& color = *static_cast<Color*>(message.getParam());
			_color.setColor(color);
		}
		else if(message.getID() == MessageID::SET_ALPHA)
		{
			float alpha = getFloatParam(message.getParam());
			_color.setA(alpha);
		}
		else if(message.getID() == MessageID::SET_TARGET)
		{
			_target = *static_cast<Vector*>(message.getParam());
		}
		else if(message.getID() == MessageID::DRAW)
		{
			LayerType::Enum layer = *static_cast<LayerType::Enum*>(message.getParam());
			if(_layer == layer)
				draw();			
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = getSerializationParam(message.getParam());
			TARGET_SERIALIZER.serialize(serialization, _target);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = getConstSerializationParam(message.getParam());
			TARGET_SERIALIZER.deserialize(serialization, _target);
		}
	}
	
	void LineRenderer::draw() const
	{
		const Vector& position = getPosition(*this);

		glPushMatrix();
		{	
			Vector vector = _target -position;
			glTranslatef(position.getX() + vector.getX() / 2.0f, position.getY() + vector.getY() / 2.0f, 0.0f);

			float length = vector.getLength() / 2.0f;
			glRotatef(vector.getAngle() * 180.0f / PI, 0.0, 0.0, 1.0f);
			
			const Size& textureSize = _texture.getSize();
			float textureWidth = textureSize.getWidth() / 2.0f;
			float textureHeight = textureSize.getHeight() / 2.0f;

			Graphics::get().bindTexture(_texture.getID());

			glColor4f(_color.getR(), _color.getG(), _color.getB(), _color.getA());

			GLfloat screenVertices[] = { -length, -textureHeight,
										 length, -textureHeight,
										 length, textureHeight,
										 -length, textureHeight};

			GLfloat textureVertices[] = { 0.0f, 0.0f,
										  1.0f, 0.0f,
										  1.0f, 1.0f, 
										  0.0f, 1.0f};
 
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
			glVertexPointer(2, GL_FLOAT, 0, screenVertices);
			glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
 
			glDrawArrays(GL_QUADS, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glPopMatrix();
	}

	Component* LineRenderer::clone() const
	{
		return new LineRenderer(_layer, _texture, _color);
	}
}