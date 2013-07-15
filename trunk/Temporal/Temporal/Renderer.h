#ifndef RENDERER_H
#define RENDERER_H
#include "Hash.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Layer.h"
#include <memory>

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	class Renderer : public Component
	{
	public:
		explicit Renderer(const char* textueFile = 0, const char* spritesheetFile = 0, SceneNode* root = 0, LayerType::Enum layer = LayerType::BACKGROUND, Color color = Color::White);
		~Renderer();

		SceneNode& getRoot() const { return *_root; }
		Hash getType() const { return TYPE; }

		void handleMessage(Message& message);
		Component* clone() const;

		static const Hash TYPE;
	private:

		const char* _textureFile;
		const char* _spriteSheetFile;
		std::shared_ptr<SpriteSheet> _spriteSheet;
		SceneNode* _root;

		LayerType::Enum _layer;
		Color _color;

		void draw() const;

		friend class SerializationAccess;
	};
}
#endif