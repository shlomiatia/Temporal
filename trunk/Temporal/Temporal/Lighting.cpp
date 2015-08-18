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
#include "Camera.h"
#include "ResourceManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <gl/glew.h>
#include <math.h>
#include <algorithm>

namespace Temporal
{
	const Hash Light::TYPE = Hash("light");
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	void Light::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			_lightTexture = ResourceManager::get().getSingleTextureSpritesheet("resources/textures/light.png");
			_shadowTexture = ResourceManager::get().getSingleTextureSpritesheet("resources/textures/shadow.png");
			getEntity().getManager().getGameState().getLayersManager().addLight(this);

		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().getGameState().getLayersManager().removeLight(this);
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
		Vector point1 = shape.getCenter() - shape.getAxisX() * shape.getRadiusX() - shape.getAxisY() * shape.getRadiusY();
		Vector point2 = shape.getCenter() + shape.getAxisX() * shape.getRadiusX() - shape.getAxisY() * shape.getRadiusY();
		Vector point3 = shape.getCenter() + shape.getAxisX() * shape.getRadiusX() + shape.getAxisY() * shape.getRadiusY();
		Vector point4 = shape.getCenter() - shape.getAxisX() * shape.getRadiusX() + shape.getAxisY() * shape.getRadiusY();

		drawShadowPart(lightCenter, point1, point2);
		drawShadowPart(lightCenter, point2, point3);
		drawShadowPart(lightCenter, point3, point4);
		drawShadowPart(lightCenter, point4, point1);
	}

	void Light::drawBeamShadow(const Vector& lightCenter)
	{
		float shadowSize = 100000.0;

		float center = _center;
		const Side::Enum entityOrientation = *static_cast<const Side::Enum*>(raiseMessage(Message(MessageID::GET_ORIENTATION)));
		if (entityOrientation == Side::LEFT)
			center = AngleUtils::radian().mirror(center);

		float angle = center - _size / 2.0f;
		Vector vector(angle);
		Vector normal = vector.getRightNormal();

		Vector obbCenter = lightCenter + normal * shadowSize / 2.0f;
		OBB obb(obbCenter, vector, Vector(shadowSize / 2.0f, shadowSize / 2.0f));
		Graphics::get().getSpriteBatch().add(&_shadowTexture->getTexture(), obb.getCenter(), AABB::Zero, Color(0.0f, 0.0f, 0.0f, 0.0f), obb.getAngle(), Vector::Zero, Vector(1.0f, 1.0f), false, false, obb.getRadius());

		angle = center + _size / 2.0f;
		vector = Vector(angle);
		normal = vector.getLeftNormal();
		obbCenter = lightCenter + normal * shadowSize / 2.0f;
		obb = OBB(obbCenter, vector, Vector(shadowSize / 2.0f, shadowSize / 2.0f));
		Graphics::get().getSpriteBatch().add(&_shadowTexture->getTexture(), obb.getCenter(), AABB::Zero, Color(0.0f, 0.0f, 0.0f, 0.0f), obb.getAngle(), Vector::Zero, Vector(1.0f, 1.0f), false, false, obb.getRadius());
	}
	
	void Light::draw()
	{
		Graphics::get().getSpriteBatch().begin();
		const Vector& position = getPosition(*this);
		glDisable(GL_BLEND);
		glColorMask(false, false, false, true);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		drawBeamShadow(position);

		OBB lightBounds = OBBAABB(position, Vector(_radius, _radius));
		int sourceCollisionGroup = getIntParam(raiseMessage(Message(MessageID::GET_COLLISION_GROUP)));
		FixtureList result = getEntity().getManager().getGameState().getGrid().iterateTiles(lightBounds, CollisionCategory::OBSTACLE, sourceCollisionGroup);

		for(FixtureIterator i = result.begin(); i != result.end(); ++i)
		{
			drawShadow(position, (**i).getGlobalShape());
		}
		Graphics::get().getSpriteBatch().end();
		glColorMask(true, true, true, true);
		glEnable(GL_BLEND);
		Graphics::get().getSpriteBatch().begin();
		Graphics::get().getSpriteBatch().add(&_lightTexture->getTexture(), position, AABB::Zero, _color, 0.0f, Vector::Zero, Vector(1.0f, 1.0f), false, false, Vector(_radius, _radius));
		Graphics::get().getSpriteBatch().end();
	}

	LightLayer::LightLayer(LayersManager* manager, const Color& ambientColor) : Layer(manager), AMBIENT_COLOR(ambientColor), _fbo(Graphics::get().getFXSpriteBatch())
	{
		_fbo.init();
	}

	void LightLayer::remove(Component* component)
	{
		_components.erase(std::remove(_components.begin(), _components.end(), component));
	}

	void LightLayer::draw(float framePeriod)
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
	}

	void LightLayer::postDraw()
	{
		void* result = getManager().getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::GET_POSITION));
		const Vector& playerPosition = *static_cast<Vector*>(result);
		
		LayersManager& layersManager = GameStateManager::get().getCurrentState().getLayersManager();
		Vector relativePosition = playerPosition - layersManager.getCamera().getBottomLeft();
		relativePosition.setX(relativePosition.getX() * Graphics::get().getResolution().getX() / Graphics::get().getLogicalView().getX());
		relativePosition.setY(relativePosition.getY() * Graphics::get().getResolution().getY() / Graphics::get().getLogicalView().getY());
		GLubyte alpha[4];
		
		glReadPixels(static_cast<int>(relativePosition.getX()), static_cast<int>(relativePosition.getY()), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &alpha);
		bool isLit = alpha[0] > AMBIENT_COLOR.getR() * 255.0f || alpha[1] > AMBIENT_COLOR.getG() * 255.0f || alpha[2] > AMBIENT_COLOR.getB() * 255.0f;
		getManager().getGameState().getEntitiesManager().sendMessageToEntity(PLAYER_ENTITY, Message(MessageID::SET_LIT, &isLit));
		_fbo.unbind();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		Graphics::get().getMatrixStack().top().reset();
		Graphics::get().getFXShaderProgram().setUniform(Graphics::get().getFXSpriteBatch().getTypeUniform(), -1);
		_fbo.draw();
		Graphics::get().getMatrixStack().top().translate(-getManager().getCamera().getBottomLeft());
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}