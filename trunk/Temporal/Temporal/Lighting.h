#ifndef LIGHTING_H
#define LIGHTING_H

#include "Color.h"
#include "EntitySystem.h"
#include "Math.h"
#include "Layer.h"
#include "FBO.h"
#include "SpriteBatch.h"
#include "ShaderProgram.h"
#include <memory>

namespace Temporal
{
	class SpriteSheet;

	/*class LightGem : public Component
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
	};*/

	class Light : public Component
	{
	public:
		explicit Light(const Color& color = Color::White, float radius = 256.0, float beamCenter = 0.0f, float beamSize = 2 * PI)
			: _color(color), _radius(radius), _beamCenter(beamCenter), _beamSize(beamSize) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new Light(_color, _radius, _beamCenter, _beamSize); }

		static const Hash TYPE;
	private:
		std::shared_ptr<SpriteSheet> _spriteSheet;

		Color _color;
		float _radius;
		float _beamCenter;
		float _beamSize;

		void draw() const;

		friend class SerializationAccess;
	};

	class LightLayer : public Layer
	{
	public:
		LightLayer(LayersManager* manager, const Color& ambientColor);
		void draw();

	private:
		const Color AMBIENT_COLOR;
		FBO _fbo;
		ShaderProgram _program;
		SpriteBatch _batch;

		void preDraw();
		void postDraw();

		LightLayer(const LightLayer&);
		LightLayer& operator=(const LightLayer&);
	};
}
#endif