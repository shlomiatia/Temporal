#include "NavigationGraph.h"
#include "NavigationNode.h"
#include "NavigationEdge.h"
#include "NavigationGraphGenerator.h"
#include "ShapeOperations.h"
#include "Graphics.h"
#include "TemporalPeriod.h"
#include "MessageUtils.h"

namespace Temporal
{
	static const Hash TIME_MACHINE_ID = Hash("ENT_TIME_MACHINE0");

	NavigationNodeList generateNavigationGraph(const GameState& gameState, Period::Enum period)
	{
		NavigationGraphGenerator navigationGraphGenerator(gameState, period);
		NavigationNodeList result = navigationGraphGenerator.get();

		return result;
	}

	void NavigationGraph::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);

		_pastNodes = generateNavigationGraph(getGameState(), Period::PAST);
		_presentNodes = generateNavigationGraph(getGameState(), Period::PRESENT);
		
		Entity* timeMachine = getGameState().getEntitiesManager().getEntity(TIME_MACHINE_ID);
		if (timeMachine)
		{
			const Vector& timeMachinePosition = getVectorParam(timeMachine->handleMessage(Message(MessageID::GET_POSITION)));
			NavigationNode* pastTimeMachineNode = getNode(timeMachinePosition, Period::PAST);
			NavigationNode* presentTimeMachineNode = getNode(timeMachinePosition, Period::PRESENT);
			pastTimeMachineNode->addEdge(new NavigationEdge(*pastTimeMachineNode, *presentTimeMachineNode, timeMachinePosition.getX(), static_cast<Side::Enum>(0), NavigationEdgeType::TEMPORAL_TRAVEL));
			presentTimeMachineNode->addEdge(new NavigationEdge(*presentTimeMachineNode, *pastTimeMachineNode, timeMachinePosition.getX(), static_cast<Side::Enum>(0), NavigationEdgeType::TEMPORAL_TRAVEL));
		}
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
		const NavigationNodeList& nodes = getNodesByPeriod(period);
		for (NavigationNodeIterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			NavigationNode* node = *i;
			if (intersects(node->getArea(), position))
				return node;
		}
		return 0;
	}

	NavigationNode* NavigationGraph::getNode(const Vector& position, int period)
	{
		return const_cast<NavigationNode*>(const_cast<const NavigationGraph*>(this)->getNode(position, period));
	}

	const NavigationNode* NavigationGraph::getNode(Hash id, int period) const
	{
		const NavigationNodeList& nodes = getNodesByPeriod(period);
		for (NavigationNodeIterator i = nodes.begin(); i != nodes.end(); ++i)
		{
			NavigationNode* node = *i;
			if (node->getId() == id)
				return node;
		}
		return 0;
	}

	NavigationNode* NavigationGraph::getNode(Hash id, int period)
	{
		return const_cast<NavigationNode*>(const_cast<const NavigationGraph*>(this)->getNode(id, period));
	}

	void NavigationGraph::drawDebug() const
	{
		
		Period::Enum playerPeriod = *static_cast<Period::Enum*>(getGameState().getEntitiesManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_COLLISION_GROUP)));
		const NavigationNodeList& nodes = getNodesByPeriod(playerPeriod);
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
	}

	const NavigationNodeList& NavigationGraph::getNodesByPeriod(int period) const
	{
		return period == Period::PAST ? _pastNodes : _presentNodes;
	}
}