#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"
#include "Layer.h"
#include "FBO.h"
#include "SpriteBatch.h"
#include "ShaderProgram.h"
#include <memory>

namespace Temporal
{
	class SpriteSheet;

	class LightGem : public Component
	{
	public:
		LightGem()
			: _isLit(false) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new LightGem(); }
		static const Hash TYPE;
	private:

		bool _isLit;
	};

	class Light : public Component
	{
	public:
		explicit Light(const Color& color = Color::White, float radius = 256.0)
			: _color(color), _radius(radius) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new Light(_color, _radius); }

		static const Hash TYPE;
	private:
		std::shared_ptr<SpriteSheet> _lightTexture;
		std::shared_ptr<SpriteSheet> _shadowTexture;

		Color _color;
		float _radius;

		void draw();
		void drawShadow(const Vector& lightCenter, const OBB& shape);
		void drawShadowPart(const Vector& lightCenter, const Vector& point1, const Vector& point2);

		friend class SerializationAccess;
	};

	class LightLayer : public Layer
	{
	public:
		LightLayer(LayersManager* manager, const Color& ambientColor);
		void add(Component* component) { _components.push_back(component); }
		void draw();

	private:
		const Color AMBIENT_COLOR;
		FBO _fbo;
		ComponentList _components;

		void preDraw();
		void postDraw();

		LightLayer(const LightLayer&);
		LightLayer& operator=(const LightLayer&);
	};
}
#endif