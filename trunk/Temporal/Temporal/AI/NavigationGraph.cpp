#include "NavigationGraph.h"
#include <Temporal\Base\Shape.h>
#include <Temporal\Base\Segment.h>
#include <Temporal\Base\ShapeOperations.h>
#include <Temporal\Base\Math.h>
#include <Temporal\Game\Message.h>
#include <Temporal\Game\Entity.h>
#include <Temporal\Game\EntitiesManager.h>
#include <Temporal\Game\ActionController.h>
#include <Temporal\Game\MovementUtils.h>
#include <Temporal\Physics\DynamicBody.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	// BRODER
	const Size NavigationGraph::MIN_AREA_SIZE = Size(32.0f, 100.0f);

	NavigationNode::~NavigationNode(void)
	{
		for(NavigationEdgeIterator i = _edges.begin(); i != _edges.end(); ++i)
		{
			delete *i;
		}
	}

	float NavigationEdge::calculateCost(const NavigationNode& source)
	{
		const Rectangle& sourceArea = source.getArea();
		const Rectangle& targetArea = getTarget().getArea();

		Segment segment(sourceArea.getCenter(), targetArea.getCenter());
		return segment.getLength();
	}

	void cutAreaLeft(const Shape& platform, const Rectangle& area, RectCollection& areas, RectIterator& iterator)
	{
		float width = area.getRight() - platform.getRight();
		const Rectangle nodeAfterCut = RectangleLB(platform.getRight(), area.getBottom(), width, area.getHeight());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreaRight(const Shape& platform, const Rectangle& area, RectCollection& areas, RectIterator& iterator)
	{
		float width = platform.getLeft() - area.getLeft();
		const Rectangle nodeAfterCut = RectangleLB(area.getLeft(), area.getBottom(), width, area.getHeight());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreasByPlatforms(RectCollection& areas, ShapeCollection& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const Shape& platform = **i;
			for(RectIterator j = areas.begin(); j != areas.end(); ++j)
			{	
				const Rectangle area = *j;
				if(intersects(area, platform))
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

	bool intersectWithPlatform(const Rectangle& area, ShapeCollection& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const Shape& platform = **i;
			if(intersects(area, platform))
			{
				return true;
			}
		}
		return false;
	}

	void NavigationGraph::createNodes(ShapeCollection& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			// Create area from platform
			const Shape& platform = **i;

			// Check min with now also, because we're going to pad it
			if(platform.getWidth() < MIN_AREA_SIZE.getWidth())
				continue;

			// Create area
			Rectangle area = RectangleLB(platform.getLeft(), platform.getTop() + 1.0f, platform.getWidth(), MIN_AREA_SIZE.getHeight());

			// Pad it a little
			// BRODER
			area = area.resize(Vector(2 * 10.0f, 0.0f));

			RectCollection areas;
			areas.push_back(area);
			cutAreasByPlatforms(areas, platforms);

			// Create nodes from areas
			for(RectIterator j = areas.begin(); j != areas.end(); ++j)
			{
				const Rectangle& area = *j;

				// Check min width
				if(area.getWidth() >= MIN_AREA_SIZE.getWidth())
					_nodes.push_back(new NavigationNode(area));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Orientation::Enum orientation, ShapeCollection& platforms)
	{
		const Rectangle& area1 = node1.getArea();
		const Rectangle& area2 = node2.getArea();
		float verticalDistance = area1.getTop() - area2.getTop();

		const Rectangle fallArea = RectangleLB(x, area2.getBottom(), 1.0f, verticalDistance);
		if(!intersectWithPlatform(fallArea, platforms))
		{
			float distance = (area2.getSide(orientation) - x) * orientation;
			float minFallDistance = getFallDistance(WALK_FORCE_PER_SECOND, DynamicBody::GRAVITY, verticalDistance);

			// BRODER
			float maxJumpHeight = getMaxJumpHeight(ANGLE_90_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY) + 80.0f;
			NavigationEdgeType::Enum type = distance < minFallDistance ? NavigationEdgeType::DESCEND : NavigationEdgeType::FALL;
			node1.addEdge(new NavigationEdge(node1, node2, x, orientation, type));
			if(verticalDistance <= maxJumpHeight)
				node2.addEdge(new NavigationEdge(node2, node1, x, Orientation::getOpposite(orientation), NavigationEdgeType::JUMP));
		}
	}

	void NavigationGraph::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, ShapeCollection& platforms)
	{
		const Rectangle& area1 = node1.getArea();
		const Rectangle& area2 = node2.getArea();
		float horizontalDistance = area2.getLeft() - area1.getRight();

		float maxJumpForwardDistance = getMaxJumpDistance(ANGLE_45_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY);
		if(area1.getBottom() == area2.getBottom() && horizontalDistance <= maxJumpForwardDistance)
		{
			Rectangle jumpArea = RectangleLB(area1.getRight(), area1.getBottom(), horizontalDistance, 1.0f);
			if(!intersectWithPlatform(jumpArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Orientation::RIGHT, NavigationEdgeType::JUMP));
				node2.addEdge(new NavigationEdge(node2, node1, area2.getLeft(), Orientation::LEFT, NavigationEdgeType::JUMP));
			}
		}
	}

	void NavigationGraph::createEdges(ShapeCollection& platforms)
	{
		// Create edges
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			NavigationNode& node1 = **i;
			const Rectangle& area1 = node1.getArea();
			for(NavigationNodeIterator j = _nodes.begin(); j != _nodes.end(); ++j)
			{
				NavigationNode& node2 = **j;
				if(&i == &j)
					continue;

				const Rectangle& area2 = node2.getArea();

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
			if(i == _nodes.end())
				break;
		}
	}

	const NavigationNode* NavigationGraph::getNodeByPosition(const Point& position) const
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
			ShapeCollection& platforms = *((ShapeCollection*)data);
			const Shape& platform = *(Shape*)entity.handleMessage(Message(MessageID::GET_SHAPE));
			platforms.push_back(&platform);
		}
	}

	void NavigationGraph::init(void)
	{
		ShapeCollection platforms;
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
			Graphics::get().draw(node.getArea(), Color::Yellow);
			const NavigationEdgeCollection& edges = node.getEdges();
			for(NavigationEdgeIterator j = edges.begin(); j != edges.end(); ++j)
			{
				const NavigationEdge& edge = **j;
				const Rectangle& area2 = edge.getTarget().getArea();
				float x1 = edge.getX();
				float x2 = edge.getType() == NavigationEdgeType::JUMP ? area2.getOppositeSide(edge.getOrientation()) : x1;
				float y1 = node.getArea().getBottom();
				float y2 = area2.getBottom();
				Graphics::get().draw(Segment(Point(x1, y1), Point(x2, y2)), Color(1.0f, 0.5f, 0.0f));
			}
		}
	}
}