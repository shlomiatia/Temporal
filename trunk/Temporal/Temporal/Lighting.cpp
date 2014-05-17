#include "Lighting.h"
#include "Vector.h"
#include "Math.h"
#include "Grid.h"
#include "StaticBody.h"
#include "Shapes.h"
#include "Texture.h"
#include "Camera.h"
#include "Fixture.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "Graphics.h"
#include <SDL.h>
#include <gl/glew.h>

namespace Temporal
{
	/*const Hash LightGem::TYPE = Hash("light-gem");
	const Hash Light::TYPE = Hash("light");

	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");
	static const int MAX_LIGHT_PARTS = 32;

	void LightGem::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_LIT)
		{
			_isLit = getBoolParam(message.getParam());
		}
		else if(message.getID() == MessageID::IS_LIT)
		{
			message.setParam(&_isLit);
		}
	}

	void Light::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::DRAW_LIGHTS)
		{
			draw();
		}
	}

	void getPoints(float x, float y, const OBB& shape, Vector& point1, Vector& point2, Vector& point3)
	{
		if(x < shape.getLeft())
		{
			if(y < -shape.getYRadius())
			{
				point1 = shape.getTopLeft();
				point2 = shape.getBottomLeft();
				point3 = shape.getBottomRight();
			}
			else if(y > shape.getYRadius())
			{
				point1 = shape.getBottomLeft();
				point2 = shape.getTopLeft();
				point3 = shape.getTopRight();
			}
			else
			{
				point1 = shape.getBottomLeft();
				point2 = shape.getTopLeft();
				point3 = shape.getTopLeft();
			}
		}
		else if(x > shape.getRight())
		{
			if(y < -shape.getYRadius())
			{
				point1 = shape.getBottomLeft();
				point2 = shape.getBottomRight();
				point3 = shape.getTopRight();
			}
			else if(y > shape.getYRadius())
			{
				point1 = shape.getTopLeft();
				point2 = shape.getTopRight();
				point3 = shape.getBottomRight();
			}
			else
			{
				point1 = shape.getBottomRight();
				point2 = shape.getTopRight();
				point3 = shape.getTopRight();
			}
		}
		else
		{
			if(y < -shape.getYRadius())
			{
				point1 = shape.getBottomLeft();
				point2 = shape.getBottomRight();
				point3 = shape.getBottomRight();				
			}
			else
			{
				point1 = shape.getTopLeft();
				point2 = shape.getTopRight();
				point3 = shape.getTopRight();
			}
		}
	}
	
	void drawShadow(const Vector& lightCenter, const OBB& shape)
	{
		// BRODER
		float shadowSize = 10000.0f;

		// Translate light and shape to origin
		const Vector& slopedRadius = shape.getSlopedRadius();
		float angle = -slopedRadius.getAngle();
		Vector relativeCenter = lightCenter - shape.getCenter();
		float rotatedY = relativeCenter.getX() * sin(angle) + relativeCenter.getY() * cos(angle);
		
		Vector point1;
		Vector point2;
		Vector point3;
		getPoints(lightCenter.getX(), rotatedY, shape, point1, point2, point3);

		Vector relativePoint1 = point1 - lightCenter;
		Vector relativePoint2 = point2 - lightCenter;
		Vector relativePoint3 = point3 - lightCenter;

		Vector vector1 = relativePoint1.normalize();
		Vector vector2 = relativePoint2.normalize();
		Vector vector3 = relativePoint3.normalize();

		float vertices[12];
		
		vertices[0] = point1.getX();
		vertices[1] = point1.getY();
		vertices[2] = point1.getX() + vector1.getX() * shadowSize;
		vertices[3] = point1.getY() + vector1.getY() * shadowSize;
		vertices[4] = point2.getX();
		vertices[5] = point2.getY();
		vertices[6] = point2.getX() + vector2.getX() * shadowSize;
		vertices[7] = point2.getY() + vector2.getY() * shadowSize;
		vertices[8] = point3.getX();
		vertices[9] = point3.getY();
		vertices[10] = point3.getX() + vector3.getX() * shadowSize;
		vertices[11] = point3.getY() + vector3.getY() * shadowSize;
		
		

		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, vertices);
 
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
 
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	
	void Light::draw() const
	{
		const Vector& position = getPosition(*this);
		Side::Enum* orientation = static_cast<Side::Enum*>(raiseMessage(Message(MessageID::GET_ORIENTATION)));
		bool isFlipped = orientation != 0 && *orientation == Side::LEFT;

		glDisable(GL_BLEND);
		glColorMask(false, false, false, true);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		OBB lightBounds = YABPAABB(position, Vector(_radius, _radius));

		FixtureCollection result = getEntity().getManager().getGameState().getGrid().iterateTiles(lightBounds, CollisionCategory::OBSTACLE);
		for(FixtureIterator i = result.begin(); i != result.end(); ++i)
		{
			drawShadow(position, (**i).getGlobalShape());
		}
		glColorMask(true, true, true, true);

		glEnable(GL_BLEND);
		glPushMatrix();
		{
			glTranslatef(position.getX(), position.getY(), 0.0f);

			int lightParts = static_cast<int>((_beamSize / (PI * 2.0f)) * MAX_LIGHT_PARTS);
			
			float vertices[(MAX_LIGHT_PARTS + 1) * 2];
			float colors[(MAX_LIGHT_PARTS + 1) * 4] = { _color.getR(), _color.getG(), _color.getB(), _color.getA() };

			vertices[0] = 0.0f;
			vertices[1] = 0.0f;

			for(int i = 0; i < lightParts; ++i)
			{
				float angle = _beamCenter - _beamSize / 2.0f + _beamSize * (static_cast<float>(i) / (static_cast<float>(lightParts - 1)));
				if(isFlipped)
					angle = PI - angle;
				vertices[i*2 + 2] = _radius * cos(angle);
				vertices[i*2 + 3] = _radius * sin(angle);
			}

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(2, GL_FLOAT, 0, vertices);
			glColorPointer(4, GL_FLOAT, 0, colors);
 
			glDrawArrays(GL_TRIANGLE_FAN, 0, lightParts + 1);
 
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}
		glPopMatrix();
	}*/

	LightLayer::LightLayer(LayersManager* manager, const Color& ambientColor) : Layer(manager), AMBIENT_COLOR(ambientColor), _fbo(Graphics::get().getSpriteBatch())
	{
		_fbo.init();
	}

	void LightLayer::draw()
	{
		preDraw();
		getManager().getGameState().getEntitiesManager().sendMessageToAllEntities(Message(MessageID::DRAW_LIGHTS));
		postDraw();
	}

	void LightLayer::preDraw()
	{
		_fbo.bind();
		glClearColor(AMBIENT_COLOR.getR(), AMBIENT_COLOR.getG(), AMBIENT_COLOR.getB(), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_DST_ALPHA, GL_ONE);
	}

	void LightLayer::postDraw()
	{
		/*void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION));
		const Vector& playerPosition = *static_cast<Vector*>(result);
		float matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		Vector translation = Vector(matrix[12], matrix[13]);
		Vector relativePosition = playerPosition + translation;
		GLubyte alpha[4];
		glReadPixels(static_cast<int>(relativePosition.getX()), static_cast<int>(relativePosition.getY()), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &alpha);
		bool isLit = alpha[0] > AMBIENT_COLOR.getR() * 255.0f || alpha[1] > AMBIENT_COLOR.getG() * 255.0f || alpha[2] > AMBIENT_COLOR.getB() * 255.0f;
		getManager().getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::SET_LIT, &isLit));*/
		_fbo.unbind();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		_fbo.draw();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}