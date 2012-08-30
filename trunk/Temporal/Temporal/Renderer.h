#ifndef RENDERER_H
#define RENDERER_H
#include "Hash.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Layer.h"

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	class Renderer : public Component
	{
	public:
		explicit Renderer(Hash spritesheetId = Hash::INVALID, Hash sceneGraphId = Hash::INVALID, LayerType::Enum layer = LayerType::BACKGROUND, Color color = Color::White) :
			_spritesheet(NULL), _root(NULL), _spritesheetId(spritesheetId), _sceneGraphId(sceneGraphId), _layer(layer), _color(color) {}
		~Renderer();

		SceneNode& getRoot() const { return *_root; }
		ComponentType::Enum getType() const { return ComponentType::RENDERER; }

		void handleMessage(Message& message);
		Component* clone() const;

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("sprite-sheet", _spritesheetId);
			serializer.serialize("scene-graph", _sceneGraphId);
			serializer.serialize("layer", (int&)_layer);
			serializer.serialize("color", _color);
		}
	private:
		const SpriteSheet* _spritesheet;
		SceneNode* _root;

		Hash _spritesheetId;
		Hash _sceneGraphId;

		LayerType::Enum _layer;
		Color _color;

		void draw() const;

	};
}
#endif