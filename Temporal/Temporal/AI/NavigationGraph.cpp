#include "NavigationGraph.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Game\Entity.h>
#include <Temporal\Game\EntitiesManager.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	const Vector NavigationGraph::MIN_AREA_SIZE = Vector(32.0f, 100.0f);
	const float NavigationGraph::MAX_JUMP_UP_DISTANCE = 150.0f;
	const float NavigationGraph::MAX_JUMP_FORWARD_DISTANCE = 300.0f;
	const float NavigationGraph::MIN_FALL_DISTANCE = 16.0f;

	NavigationNode::~NavigationNode(void)
	{
		for(NavigationEdgeIterator i = _edges.begin(); i != _edges.end(); ++i)
		{
			delete *i;
		}
	}

	// TODO: Consider transition type AI
	float NavigationEdge::calculateCost(const NavigationNode& source)
	{
		const Rect& sourceArea = source.getArea();
		const Rect& targetArea = getTarget().getArea();

		float x1 = sourceArea.getCenterX();
		float x2 = targetArea.getCenterX();
		float y1 = sourceArea.getCenterY();
		float y2 = targetArea.getCenterY();

		return sqrt(pow(x2-x1, 2.0f) + pow(y2-y1, 2.0f));
	}

	void cutAreaLeft(const Rect& platform, const Rect& area, RectCollection& areas, RectIterator& iterator)
	{
		float width = area.getRight() - platform.getRight() + 1.0f;
		const Rect nodeAfterCut = RectLB(platform.getRight(), area.getBottom(), width, area.getHeight());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreaRight(const Rect& platform, const Rect& area, RectCollection& areas, RectIterator& iterator)
	{
		float width = platform.getLeft() - area.getLeft() + 1.0f;
		const Rect nodeAfterCut = RectLB(area.getLeft(), area.getBottom(), width, area.getHeight());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreasByPlatforms(RectCollection& areas, RectCollection& platforms)
	{
		for(RectIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const Rect& platform = *i;
			for(RectIterator j = areas.begin(); j != areas.end(); ++j)
			{	
				const Rect area = *j;
				if(area.intersectsExclusive(platform))
				{
					j = areas.erase(j);
					if(platform.getLeft() <= area.getLeft() && platform.getRight() <= area.getRight())
					{
						cutAreaLeft(platform, area, areas, j);
					}
					else if(platform.getLeft() >= area.getLeft() && platform.getRight() >= area.getRight())
					{
						cutAreaRight(platform, area, areas, j);
					}
					else if(platform.getLeft() >= area.getLeft() && platform.getRight() <= area.getRight())
					{
						cutAreaLeft(platform, area, areas, j);
						cutAreaRight(platform, area, areas, j);
					}
					else if(j == areas.end())
					{
						break;
					}
				}
			}
		}
	}

	bool intersectWithPlatform(const Rect& area, RectCollection& platforms)
	{
		for(RectIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const Rect& platform = *i;
			if(platform.intersectsExclusive(area))
			{
				return true;
			}
		}
		return false;
	}

	void NavigationGraph::createNodes(RectCollection& platforms)
	{
		for(RectIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			// Create area from platform
			const Rect& platform = *i;

			// Check min with now also, because we're goinf to pad it
			if(platform.getWidth() < MIN_AREA_SIZE.getWidth())
				continue;

			// Create area
			Rect area = RectCB(platform.getCenterX(), platform.getTop(), platform.getWidth(), MIN_AREA_SIZE.getHeight());

			// Pad it a little
			area = Rect(area.getCenterX(), area.getCenterY(), area.getWidth() + 2 * MIN_FALL_DISTANCE, area.getHeight());

			RectCollection areas;
			areas.push_back(area);
			cutAreasByPlatforms(areas, platforms);

			// TODO: Handle areas intersection PHYISICS

			// Create nodes from areas
			for(RectIterator j = areas.begin(); j != areas.end(); ++j)
			{
				const Rect& area = *j;

				// Check min width
				if(area.getWidth() >= MIN_AREA_SIZE.getWidth())
					_nodes.push_back(new NavigationNode(area));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Orientation::Enum orientation, RectCollection& platforms)
	{
		const Rect& area1 = node1.getArea();
		const Rect& area2 = node2.getArea();
		float verticalDistance = area1.getTop() - area2.getTop();

		const Rect& fallArea = RectLB(x, area2.getBottom(), 1.0f, verticalDistance);
		if(!intersectWithPlatform(fallArea, platforms))
		{
			float distance = (area2.getSide(orientation) - x) * orientation;
			NavigationEdgeType::Enum type = distance < MIN_FALL_DISTANCE ? NavigationEdgeType::DESCEND : NavigationEdgeType::FALL;
			node1.addEdge(new NavigationEdge(node1, node2, x, orientation, type));
			if(verticalDistance <= MAX_JUMP_UP_DISTANCE)
				node2.addEdge(new NavigationEdge(node2, node1, x, Orientation::getOpposite(orientation), NavigationEdgeType::JUMP));
		}
	}

	void NavigationGraph::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, RectCollection& platforms)
	{
		const Rect& area1 = node1.getArea();
		const Rect& area2 = node2.getArea();
		float horizontalDistance = area2.getLeft() - area1.getRight();

		// TODO: Support directed jump at jump forward PHYSICS
		if(area1.getBottom() == area2.getBottom() && horizontalDistance <= MAX_JUMP_FORWARD_DISTANCE)
		{
			Rect jumpArea = RectLB(area1.getRight(), area1.getBottom(), horizontalDistance, 1.0f);
			if(!intersectWithPlatform(jumpArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Orientation::RIGHT, NavigationEdgeType::JUMP));
				node2.addEdge(new NavigationEdge(node2, node1, area2.getLeft(), Orientation::LEFT, NavigationEdgeType::JUMP));
			}
		}
	}

	void NavigationGraph::createEdges(RectCollection& platforms)
	{
		// Create edges
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			NavigationNode& node1 = **i;
			const Rect& area1 = node1.getArea();
			for(NavigationNodeIterator j = _nodes.begin(); j != _nodes.end(); ++j)
			{
				NavigationNode& node2 = **j;
				if(&i == &j)
					continue;

				const Rect& area2 = node2.getArea();

				// check jump forward
				if(area1.getRight() < area2.getLeft())
				{
					checkHorizontalEdges(node1, node2, platforms);
				}
				// check fall/jump up
				else if(area1.getTop() > area2.getTop() && area1.getLeft() <= area2.getRight() && area1.getRight() >= area2.getLeft())
				{
					if(area1.getLeft() >= area2.getLeft())
						checkVerticalEdges(node1, node2, area1.getLeft(), Orientation::LEFT, platforms);
					if(area1.getRight() <= area2.getRight())
						checkVerticalEdges(node1, node2, area1.getRight(), Orientation::RIGHT, platforms);
				}
			}
		}

		// Remove nodes without edges
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			if(node.getEdges().size() == 0)
				i = _nodes.erase(i);
		}
	}

	const NavigationNode* NavigationGraph::getNodeByPosition(const Vector& position) const
	{
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode* node = *i;
			if(node->getArea().contains(position))
				return node;
		}
		return NULL;
	}

	void addPlatform(const Entity& entity, void* data) 
	{
		bool isCover = *(bool*)entity.handleMessage(Message(MessageID::IS_COVER));
		if(!isCover)
		{
			RectCollection& platforms = *((RectCollection*)data);
			Rect platform(Rect::Empty);
			entity.handleMessage(Message(MessageID::GET_BOUNDS, &platform));
			platforms.push_back(platform);
		}
	}

	void NavigationGraph::init(void)
	{
		RectCollection platforms;
		EntitiesManager::get().iterateEntities(ComponentType::STATIC_BODY, &platforms, &addPlatform);
		createNodes(platforms);
		createEdges(platforms);
	}

	void NavigationGraph::dispose(void)
	{
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			delete *i;
		}
	}

	void NavigationGraph::draw(void) const
	{
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			Graphics::get().drawRect(node.getArea(), Color::Yellow);
			const NavigationEdgeCollection& edges = node.getEdges();
			for(NavigationEdgeIterator j = edges.begin(); j != edges.end(); ++j)
			{
				const NavigationEdge& edge = **j;
				const Rect& area2 = edge.getTarget().getArea();
				float x1 = edge.getX();
				float x2 = edge.getType() == NavigationEdgeType::JUMP ? area2.getOppositeSide(edge.getOrientation()) : x1;
				float y1 = node.getArea().getBottom();
				float y2 = area2.getBottom();
				Graphics::get().drawLine(Vector(x1, y1), Vector(x2, y2), Color(1.0f, 0.5f, 0.0f));
			}
		}
	}
}