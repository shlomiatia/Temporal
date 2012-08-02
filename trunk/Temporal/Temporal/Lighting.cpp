#include "Lighting.h"
#include "NumericPair.h"
#include "Math.h"
#include "Grid.h"
#include "StaticBody.h"
#include "Shapes.h"
#include "Texture.h"
#include "ViewManager.h"
#include <SDL.h>
#include <SDL_opengl.h>

namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");
	static const int MAX_LIGHT_PARTS = 32;

	void LightGem::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_LIT)
		{
			_isLit = *static_cast<bool*>(message.getParam());
		}
		else if(message.getID() == MessageID::IS_LIT)
		{
			message.setParam(&_isLit);
		}
	}

	void Light::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(layer == VisualLayer::LIGHT)
				draw();
		}
	}

	bool drawShadows(void* caller, void* data, const StaticBody& staticBody)
	{
		if(!staticBody.isCover())
		{
			float shadowSize = 10000.0f;
			const Vector& lightCenter = *static_cast<const Vector*>(data);
			const Segment& segment = static_cast<const Segment&>(staticBody.getShape());
			Point leftPoint = segment.getLeftPoint();
			Point rightPoint = segment.getRightPoint();
			Vector vector1 = Vector(leftPoint - lightCenter).normalize();
			Vector vector2 = Vector(rightPoint - lightCenter).normalize();

			float vertices[8];
			
			vertices[0] = leftPoint.getX() + vector1.getVx() * shadowSize;
			vertices[1] = leftPoint.getY() + vector1.getVy() * shadowSize;
			vertices[2] = leftPoint.getX();
			vertices[3] = leftPoint.getY();
			vertices[4] = rightPoint.getX();
			vertices[5] = rightPoint.getY();
			vertices[6] = rightPoint.getX() + vector2.getVx() * shadowSize;
			vertices[7] = rightPoint.getY() + vector2.getVy() * shadowSize;

			glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(2, GL_FLOAT, 0, vertices);
 
			glDrawArrays(GL_QUADS, 0, 4);
 
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		return true;
	}

	void Light::draw() const
	{
		Point& position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		Side::Enum* orientation = static_cast<Side::Enum*>(sendMessageToOwner(Message(MessageID::GET_ORIENTATION)));
		bool isFlipped = orientation != NULL && *orientation == Side::LEFT;

		glDisable(GL_BLEND);
		glColorMask(false, false, false, true);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		AABB lightAABB = AABB(position.getX(), position.getY(), _radius * 2.0f, _radius * 2.0f);
		Grid::get().iterateTiles(lightAABB, 0, NULL, static_cast<void*>(&position), drawShadows);
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
	}

	void LightSystem::init(const Size& size)
	{
		_texture = Texture::load(size);
	}

	void LightSystem::preLightsDraw() const
	{
		glClearColor(AMBIENT_COLOR.getR(), AMBIENT_COLOR.getG(), AMBIENT_COLOR.getB(), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_DST_ALPHA, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void LightSystem::postLightsDraw() const
	{
		const Point& playerPosition = *static_cast<Point*>(EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION)));
		Point relativePosition = playerPosition - ViewManager::get().getCameraBottomLeft();
		GLubyte alpha[4];
		glReadPixels(static_cast<int>(relativePosition.getX()), static_cast<int>(relativePosition.getY()), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &alpha);
		bool isLit = alpha[0] > AMBIENT_COLOR.getR() * 255.0f || alpha[1] > AMBIENT_COLOR.getG() * 255.0f || alpha[2] > AMBIENT_COLOR.getB() * 255.0f;
		EntitiesManager::get().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::SET_LIT, &isLit));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, _texture->getID()); 
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, static_cast<int>(_texture->getSize().getWidth()), static_cast<int>(_texture->getSize().getHeight()), 0);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void LightSystem::postDraw() const
	{
		glLoadIdentity();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);

		glBindTexture(GL_TEXTURE_2D, _texture->getID());
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		GLfloat screenVertices[] = { 0.0f, 0.0f,
									 0.0f, _texture->getSize().getHeight(),
									 _texture->getSize().getWidth(), _texture->getSize().getHeight(),
									 _texture->getSize().getWidth(), 0.0f };

		GLfloat textureVertices[] = { 0.0f, 0.0f,
									  0.0f, 1.0f,
								      1.0f, 1.0f,
									  1.0f, 0.0f };

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
		glVertexPointer(2, GL_FLOAT, 0, screenVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, textureVertices);
 
		glDrawArrays(GL_QUADS, 0, 4);
 
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}