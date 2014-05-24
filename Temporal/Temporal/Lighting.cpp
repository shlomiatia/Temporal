#include "Lighting.h"
#include "Vector.h"
#include "Math.h"
#include "Grid.h"
#include "StaticBody.h"
#include "Shapes.h"
#include "SpriteSheet.h"
#include "Fixture.h"
#include "MessageUtils.h"
#include "PhysicsEnums.h"
#include "Graphics.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <gl/glew.h>
#include <math.h>

namespace Temporal
{
	const Hash Light::TYPE = Hash("light");
	static const int MAX_LIGHT_PARTS = 32;

	/*const Hash LightGem::TYPE = Hash("light-gem");
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");
	

	/*void LightGem::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_LIT)
		{
			_isLit = getBoolParam(message.getParam());
		}
		else if(message.getID() == MessageID::IS_LIT)
		{
			message.setParam(&_isLit);
		}
	}*/

	void Light::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_lightTexture = ResourceManager::get().getSingleTextureSpritesheet("resources/textures/light.png");
			_shadowTexture = ResourceManager::get().getSingleTextureSpritesheet("resources/textures/shadow.png");
			getEntity().getManager().getGameState().getLayersManager().addLight(this);

		}
		else if(message.getID() == MessageID::DRAW_LIGHTS)
		{
			draw();
		}
	}

	void Light::drawShadowPart(const Vector& lightCenter, const Vector& point1, const Vector& point2)
	{
		float shadowSize = 100000.0f;
		Vector point3 = point2 + (point2 - lightCenter).normalize() * shadowSize;
		Vector point4 = point1 + (point1 - lightCenter).normalize() * shadowSize;

		Graphics::get().getSpriteBatch().add(&_shadowTexture->getTexture(), point1, point2, point3, point4, AABB::Zero, Color(0.0f, 0.0f, 0.0f, 0.0f));

	}
	
	void Light::drawShadow(const Vector& lightCenter, const OBB& shape)
	{
		Vector point1 = shape.getPoint(Axis::Y, false);
		Vector point2 = shape.getPoint(Axis::X, true);
		Vector point3 = shape.getPoint(Axis::Y, true);
		Vector point4 = shape.getPoint(Axis::X, false);

		drawShadowPart(lightCenter, point1, point2);
		drawShadowPart(lightCenter, point2, point3);
		drawShadowPart(lightCenter, point3, point4);
		drawShadowPart(lightCenter, point4, point1);
	}
	
	void Light::draw()
	{
		Graphics::get().getSpriteBatch().begin();
		const Vector& position = getPosition(*this);
		glDisable(GL_BLEND);
		glColorMask(false, false, false, true);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		OBB lightBounds = OBBAABB(position, Vector(_radius, _radius));

		FixtureCollection result = getEntity().getManager().getGameState().getGrid().iterateTiles(lightBounds, CollisionCategory::OBSTACLE);

		for(FixtureIterator i = result.begin(); i != result.end(); ++i)
		{
			drawShadow(position, (**i).getGlobalShape());
		}
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getSpriteBatch().begin();
		glColorMask(true, true, true, true);
		glEnable(GL_BLEND);
		Graphics::get().getSpriteBatch().add(&_lightTexture->getTexture(), position, AABB::Zero, _color, 0.0f, Vector::Zero, Vector(1.0f, 1.0f), false, false, Vector(_radius, _radius));
		Graphics::get().getSpriteBatch().end();
	}

	LightLayer::LightLayer(LayersManager* manager, const Color& ambientColor) : Layer(manager), AMBIENT_COLOR(ambientColor), _batch(_program), _fbo(_batch)
	{
		_program.init("resources/shaders/v.glsl", "resources/shaders/f.glsl");
		glm::mat4 projection = glm::ortho(0.0f, Graphics::get().getResolution().getX(), Graphics::get().getResolution().getY(), 0.0f, -1.0f, 1.0f);
		_program.setUniform(_program.getUniform("u_projection"), glm::value_ptr(projection));
		_batch.init();
		_fbo.init();
	}

	void LightLayer::draw()
	{
		preDraw();
		for(ComponentIterator i = _components.begin(); i != _components.end(); ++i)
		{
			(**i).handleMessage(Message(MessageID::DRAW_LIGHTS, this));
		}
		postDraw();
	}

	void LightLayer::preDraw()
	{
		_fbo.bind();
		glClearColor(AMBIENT_COLOR.getR(), AMBIENT_COLOR.getG(), AMBIENT_COLOR.getB(), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_DST_ALPHA, GL_ONE);
		Graphics::get().getSpriteBatch().begin();
	}

	void LightLayer::postDraw()
	{
		Graphics::get().getSpriteBatch().end();
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