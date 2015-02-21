#ifndef ANIMATION_EDITOR_H
#define ANIMATION_EDITOR_H
#include "Hash.h"
#include "EntitySystem.h"
#include "Vector.h"
#include "Keyboard.h"
#include "Animation.h"
#include "Delegate.h"
#include <memory>
#include <vector>

// Animation editor
namespace Temporal
{
	class SceneNode;
	class MouseParams;
	class AABB;
	class Control;

	namespace SceneNodeAction
	{
		enum Enum
		{
			NONE,
			CREATE,
			DELETE
		};
	}

	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offsetTranslation(Vector::Zero), _copyAnimation(Hash::INVALID), _translation(false), _rotation(false), _move(false), _paused(false), _index(0), _copyIndex(0), GRID_FRAMES(0), _offsetAngle(0.0f) {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		static const float FRAME_TIME;

		int GRID_FRAMES;

		std::shared_ptr<AnimationSet> _animationSet;
		Hash _animationId;
		HashCollection _sceneNodes;
		Hash _sceneNodeId;
		int _index;
		bool _paused;

		Vector _offsetTranslation;
		float _offsetAngle;
		bool _translation;
		bool _rotation;
		bool _move;
		std::vector<AnimationSet*> _undo;
		Hash _copyAnimation;
		int _copyIndex;

		void setAnimation(Hash animationId);
		void setFrame();
		void setSceneGraphIndex(int index);
		void setSceneNodeId(Hash sceneNodeId);
		void updateGrid();
		void bindSceneNodes(SceneNode& sceneNode);

		Hash getSnsId(int index, Hash snId);
		SceneGraphSample* getSceneGraphSample(int index = -1, SceneNodeAction::Enum action = SceneNodeAction::NONE, Hash animationId = Hash::INVALID);
		SceneNodeSample* getSceneNodeSample(int index = -1, Hash sceneNodeId = Hash::INVALID, SceneNodeAction::Enum action = SceneNodeAction::NONE, Hash animationId = Hash::INVALID);
		SceneNodeSample& getCreateSceneNodeSample(int index = -1, Hash sceneNodeId = Hash::INVALID, Hash animationId = Hash::INVALID);
		SceneNodeSample* addSample(SceneNodeSampleCollection& samples, Hash sceneNodeId);
		Vector getParentGlobalPosition();
		Vector getGlobalPosition();
		const SceneNode& getSceneNode(Hash sceneNodeId = Hash::INVALID);
		void setRotation(float rotation);
		Vector getTranslation(const Vector& translation);

		void handleArrows(const Vector& vector);
		void addUndo();
		void undo();
		void save();
		void newAnimation(const char* name);
		void deleteAnimation();
		void nextAnimation();
		void previousAnimation();
		void toggleAnimation();
		void copy();
		void paste();
		void newSgs();
		void deleteSns();

		void handleKey(Key::Enum key);
		void init();
		void update();

		void skeletonLeftMouseDown(const MouseParams& params);
		void skeletonMiddleMouseDown(const MouseParams& params);
		void skeletonRightMouseDown(const MouseParams& params);
		void skeletonMouseMove(const MouseParams& params);
		void snsLeftClick(const MouseParams& params);

		Control* addControl(Hash id, const AABB& shape);
		Control* addLabel(const AABB& shape, const char* text);
		Control* addButton(Hash id, const AABB& shape, const char* text, IAction* commandEvent, Key::Enum shortcutKey = Key::NONE);
		Control* addTextBox(Hash id, const AABB& shape, const char* text, IAction1<const char*>* textChangedEvent);
	};

	class AEGameStateListener : public GameStateListener
	{
	public:
		void onLoaded(Hash id, GameState& gameState);
	};
}
#endif