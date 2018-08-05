#ifndef RENDERER_H
#define RENDERER_H
#include "Hash.h"
#include "Color.h"
#include "EntitySystem.h"
#include "LayersManager.h"
#include <memory>
#include "Ids.h"

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	class Renderer : public Component
	{
	public:
		explicit Renderer(const char* textueFile = "", const char* spritesheetFile = "", SceneNode* root = 0, LayerType::Enum layer = LayerType::BACKGROUND, Color color = Color::White, bool visibile = true);
		~Renderer();

		Hash getType() const { return ComponentsIds::RENDERER; }

		void handleMessage(Message& message);
		Component* clone() const;

		const SceneNode& getRootSceneNode() const { return *_root; }
		SceneNode& getRootSceneNode() { return *_root; }
		SpriteSheet& getSpriteSheet() const { return *_spriteSheet; }
		LayerType::Enum getLayer() const { return _layer; }
	private:
		std::string _textureFile;
		std::string _spriteSheetFile;
		std::shared_ptr<SpriteSheet> _spriteSheet;
		SceneNode* _root;

		LayerType::Enum _layer;
		Color _color;
		bool _visible;

		void draw();
		void draw(const SceneNode& sceneNode);

		friend class SerializationAccess;
	};
}
#endif