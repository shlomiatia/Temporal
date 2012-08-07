#include "LineRenderer.h"
#include "Graphics.h"
#include "Serialization.h"
#include "BaseUtils.h"
#include "Shapes.h"
#include "Math.h"

#include "Texture.h"
#include <SDL_opengl.h>

namespace Temporal
{
	static const NumericPairSerializer TARGET_SERIALIZER("LRE_SER_TAR");

	void LineRenderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLOR)
		{
			_color = *static_cast<Color*>(message.getParam());
		}
		else if(message.getID() == MessageID::SET_TARGET)
		{
			_target = *static_cast<Point*>(message.getParam());
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(_layer == layer)
				draw();			
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *static_cast<Serialization*>(message.getParam());
			TARGET_SERIALIZER.serialize(serialization, _target);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *static_cast<const Serialization*>(message.getParam());
			TARGET_SERIALIZER.deserialize(serialization, _target);
		}
	}
	
	void LineRenderer::draw() const
	{
		const Point& position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		//Graphics::get().draw(Segment(position, _target), _color);

		glPushMatrix();
		{	
			Vector vector = _target -position;
			glTranslatef(position.getX() + vector.getVx() / 2.0f, position.getY() + vector.getVy() / 2.0f, 0.0f);

			
			float length = vector.getLength() / 2.0f;
			glRotatef(vector.getAngle() * 180.0f / PI, 0.0, 0.0, 1.0f);
			
			const Size& textureSize = _texture.getSize();
			float textureWidth = textureSize.getWidth() / 2.0f;
			float textureHeight = textureSize.getHeight() / 2.0f;

			glBindTexture(GL_TEXTURE_2D, _texture.getID());

			glColor4f(_color.getR(), _color.getG(), _color.getB(), _color.getA());

			GLfloat screenVertices[] = { -length, -textureHeight,
										 length, -textureHeight,
										 length, textureHeight,
										 -length, textureHeight};

			GLfloat textureVertices[] = { -length, 0.0f,
										  length, 0.0f,
										  length, 1.0f, 
										  -length, 1.0f};
 
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