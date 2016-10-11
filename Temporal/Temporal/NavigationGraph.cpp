#include "NavigationGraph.h"
#include "NavigationNode.h"
#include "NavigationEdge.h"
#include "NavigationGraphGenerator.h"
#include "ShapeOperations.h"
#include "DynamicBody.h"
#include "StaticBody.h"
#include "Fixture.h"
#include "Graphics.h"
#include "PhysicsEnums.h"
#include "TemporalPeriod.h"

namespace Temporal
{
	void addPlatforms(const HashEntityMap& entities, OBBList& platforms, Period::Enum periodType)
	{
		for (HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
		{
			const Entity& entity = *(i->second);
			const StaticBody* body = static_cast<const StaticBody*>(entity.get(StaticBody::TYPE));
			const TemporalPeriod* period = static_cast<const TemporalPeriod*>(entity.get(TemporalPeriod::TYPE));
			if (body)
			{
				if (period && period->getPeriod() != periodType)
				{
					continue;
				}
				if (body->getFixture().getCategory() == CollisionCategory::OBSTACLE)
				{
					const OBB& platform = body->getFixture().getGlobalShape();
					platforms.push_back(platform);
				}
			}

		}
	}

	void NavigationGraph::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);
		const HashEntityMap& entities = getGameState().getEntitiesManager().getEntities();
		OBBList pastPlatforms;
		addPlatforms(entities, pastPlatforms, Period::PAST);
		NavigationGraphGenerator pastGenerator(getGameState().getGrid(), pastPlatforms);
		_pastNodes = pastGenerator.get();

		OBBList presentPlatforms;
		addPlatforms(entities, presentPlatforms, Period::PRESENT);
		NavigationGraphGenerator presentGenerator(getGameState().getGrid(), presentPlatforms);
		_presentNodes = presentGenerator.get();
	}

	NavigationGraph::~NavigationGraph()
	{
		for (NavigationNodeIterator i = _pastNodes.begin(); i != _pastNodes.end(); ++i)
		{
			delete *i;
		}
		for (NavigationNodeIterator i = _presentNodes.begin(); i != _presentNodes.end(); ++i)
		{
			delete *i;
		}
	}

	const NavigationNode* NavigationGraph::getNode(const Vector& position, int period) const
	{
		const NavigationNodeList& nodes = static_cast<Period::Enum>(period) == Period::PAST ? _pastNodes : _presentNodes;
		for (NavigationNodeIterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			const NavigationNode* node = *i;
			if (intersects(node->getArea(), position))
				return node;
		}
		return 0;
	}

	void NavigationGraph::draw() const
	{
		
		Period::Enum playerPeriod = *static_cast<Period::Enum*>(getGameState().getEntitiesManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_COLLISION_GROUP)));
		const NavigationNodeList& nodes = playerPeriod == Period::PAST ? _pastNodes : _presentNodes;
		Graphics::get().getLinesSpriteBatch().begin();
		for (NavigationNodeIterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			Graphics::get().getLinesSpriteBatch().add(node.getArea(), Color::Yellow);
		}
		for (NavigationNodeIterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			const NavigationEdgeList& edges = node.getEdges();
			for(NavigationEdgeIterator j = edges.begin(); j != edges.end(); ++j)
			{
				const NavigationEdge& edge = **j;
				const OBB& area2 = edge.getTarget().getArea();
				float x1 = edge.getX();
				float x2 = (edge.getType() == NavigationEdgeType::JUMP_FORWARD) ? area2.getSide(Side::getOpposite(edge.getSide())) : x1;
				float y1 = edge.getType() == NavigationEdgeType::WALK ? node.getArea().getCenterY() : node.getArea().getBottom();
				float y2 = edge.getType() == NavigationEdgeType::WALK ? area2.getCenterY() : area2.getBottom();
				Color color = Color::White;
				if(edge.getType() == NavigationEdgeType::JUMP_UP)
				{
					color = Color(1.0f, 0.5f, 0.0f);
					
				}
				else if(edge.getType() == NavigationEdgeType::JUMP_FORWARD)
				{
					color = Color::Green;
				}
				else if(edge.getType() == NavigationEdgeType::DESCEND)
				{
					color = Color::Blue;
					x1 -= 5.0f;
					x2 -= 5.0f;
				}
				else if(edge.getType() == NavigationEdgeType::FALL)
				{
					color = Color::Cyan;
					x1 += 5.0f;
					x2 += 5.0f;
				}
				else
				{
					color = Color::Magenta;
				}
				Segment segment = SegmentPP(Vector(x1, y1), Vector(x2, y2));
				Vector segmentRadius = segment.getRadius();
				Vector axis = segmentRadius.normalize();
				axis = axis == Vector::Zero ? Vector(1.0f, 0.0f) : axis;
				Vector radius  = Vector(segmentRadius.getLength(), 1.0);
				OBB obb = OBB(segment.getCenter(), axis, radius);
				Graphics::get().getLinesSpriteBatch().add(obb, color);
			}
		}
		Graphics::get().getLinesSpriteBatch().end();
	}
}