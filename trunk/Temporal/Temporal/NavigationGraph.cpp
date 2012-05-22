#include "NavigationGraph.h"
#include "Shape.h"
#include "Segment.h"
#include "ShapeOperations.h"
#include "Math.h"
#include "Message.h"
#include "Entity.h"
#include "EntitiesManager.h"
#include "ActionController.h"
#include "MovementUtils.h"
#include "DynamicBody.h"
#include "StaticBody.h"
#include "Graphics.h"
#include "DirectedInterval.h"

namespace Temporal
{
	// BRODER
	const Size NavigationGraph::MIN_AREA_SIZE = Size(32.0f, 96.0f);

	NavigationNode::~NavigationNode(void)
	{
		for(NavigationEdgeIterator i = _edges.begin(); i != _edges.end(); ++i)
		{
			delete *i;
		}
	}

	float NavigationEdge::calculateCost(const NavigationNode& source)
	{
		const YABP& sourceArea = source.getArea();
		const YABP& targetArea = getTarget().getArea();

		Segment segment(sourceArea.getCenter(), targetArea.getCenter());
		return segment.getLength();
	}

	void cutArea(Side::Enum direction, float cutAmount, const YABP& area, YABPCollection& areas, YABPIterator& iterator)
	{
		Vector normalizedSlopedVector = area.getSlopedRadius().normalize();
		float length = cutAmount / normalizedSlopedVector.getVx();
		Vector cutRadius = (normalizedSlopedVector * length) / 2.0f;

		// Move center in the opposite direction
		Point center = area.getCenter() + (float)Side::getOpposite(direction) * cutRadius;
		Vector slopedRadius = area.getSlopedRadius() - cutRadius;
		if(slopedRadius.getVx() > 0.0f)
		{
			const YABP nodeAfterCut = YABP(center, slopedRadius, area.getYRadius());
			iterator = areas.insert(iterator, nodeAfterCut);
		}
	}

	void cutAreaLeft(float x, const YABP& area, YABPCollection& areas, YABPIterator& iterator)
	{
		float amount = x - area.getLeft();
		cutArea(Side::LEFT, amount, area, areas, iterator);
	}

	void cutAreaRight(float x, const YABP& area, YABPCollection& areas, YABPIterator& iterator)
	{
		float amount = area.getRight() - x;
		cutArea(Side::RIGHT, amount, area, areas, iterator);
	}

	Segment getUpperSegment(const YABP& yabp)
	{
		return Segment(yabp.getCenter() + yabp.getYVector(), yabp.getSlopedRadius());
	}

	Segment getLowerSegment(const YABP& yabp)
	{
		return Segment(yabp.getCenter() - yabp.getYVector(), yabp.getSlopedRadius());
	}

	void updateMinMax(const Point& point, const Vector& segmentVector, const Vector& slopedRadius, float& val1, float& val2)
	{
		if(point != Vector::Zero)
		if(segmentVector.getVy() > 0.0f || (segmentVector.getVy() == 0.0f && slopedRadius.getVy() < 0.0f))
			val1 = point.getX();
		else
			val2 = point.getX();	
	}

	void cutAreasByPlatforms(YABPCollection& areas, ShapeCollection& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const Shape& platform = **i;
			for(YABPIterator j = areas.begin(); j != areas.end(); ++j)
			{	
				const YABP area = *j;
				if(intersects(area, platform))
				{
					j = areas.erase(j);
					Vector yVector = area.getYVector();
					const Vector& slopedRadius = area.getSlopedRadius();
					Segment upperSeg = getUpperSegment(area);
					DirectedSegment upperSlope = DirectedSegment(upperSeg.getNaturalOrigin(), upperSeg.getNaturalVector());
					Segment lowerSeg = getLowerSegment(area);
					DirectedSegment lowerSlope = DirectedSegment(lowerSeg.getNaturalOrigin(), lowerSeg.getNaturalVector());
					Point upperSlopePoint = Point::Zero;
					Point lowerSlopePoint = Point::Zero;
					intersects(upperSlope, platform, &upperSlopePoint);
					intersects(lowerSlope, platform, &lowerSlopePoint);
					float min = platform.getLeft();
					float max = platform.getRight();
					const Segment& segment = (const Segment&)platform;
					Vector segmentVector = segment.getNaturalVector();
					updateMinMax(upperSlopePoint, segmentVector, slopedRadius, max, min);
					updateMinMax(lowerSlopePoint, segmentVector, slopedRadius, min, max);
					if(max >= area.getLeft() && max <= area.getRight())
						cutAreaLeft(max + 1.0f, area, areas, j);
					if(min >= area.getLeft() && min <= area.getRight())
						cutAreaRight(min - 1.0f, area, areas, j);
					if(j == areas.end())
					{
						break;
					}
				}
			}
		}
	}

	bool intersectWithPlatform(const DirectedSegment& area, ShapeCollection& platforms)
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

	void removeNodesWithoutEdges(NavigationNodeCollection& nodes)
	{
		for(NavigationNodeIterator i = nodes.begin(); i != nodes.end();)
		{
			const NavigationNode& inode = **i;
			bool haveEdge = inode.getEdges().size() > 0;

			// Check if any edge is directed to this node
			if(!haveEdge)
			{
				for(NavigationNodeIterator j = nodes.begin(); j != nodes.end(); ++j)
				{
					const NavigationNode& jnode = **j;
					for(NavigationEdgeIterator k = jnode.getEdges().begin(); k != jnode.getEdges().end(); ++k)
					{
						const NavigationEdge& edge = **k;
						if(&(edge.getTarget()) == &inode)
						{
							haveEdge = true;
							break;
						}
					}
					if(haveEdge)
						break;
				}
			}
				
			if(!haveEdge)
			{
				delete *i;
				i = nodes.erase(i);
			}
			else
				++i;
		}
	}

	const NavigationNode* NavigationGraph::getNodeByAABB(const AABB& aabb) const
	{
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode* node = *i;
			if(intersects(node->getArea(),  aabb))
				return node;
		}
		return NULL;
	}

	void NavigationGraph::createNodes(ShapeCollection& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			// Create area from platform
			const Shape& platform = **i;
			const Segment& segment = (const Segment&)platform;
			Vector vector = segment.getNaturalVector();
			float angle = vector.getAngle();
			if(!isModerateAngle(angle))
				continue;

			// Create area
			Point center = Point(segment.getCenterX(), segment.getCenterY() + 1.0f + MIN_AREA_SIZE.getHeight() / 2.0f);
			Vector slopedRadius = vector / 2.0f;
			float yRadius = MIN_AREA_SIZE.getHeight() / 2.0f;
			YABP area = YABP(center, slopedRadius, yRadius);

			YABPCollection areas;
			areas.push_back(area);
			cutAreasByPlatforms(areas, platforms);
			
			// Create nodes from areas
			for(YABPIterator j = areas.begin(); j != areas.end(); ++j)
			{
				const YABP& area = *j;

				// Check min width
				if(area.getSlopedRadius().getLength() * 2.0f >= MIN_AREA_SIZE.getWidth())
					_nodes.push_back(new NavigationNode(area));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Side::Enum orientation, ShapeCollection& platforms)
	{
		const YABP& area1 = node1.getArea();
		const YABP& area2 = node2.getArea();
		Segment lowerSegment1 = getLowerSegment(area1);
		Segment lowerSegment2 = getLowerSegment(area2);
		float y1 = lowerSegment1.getY(x);
		float y2 = lowerSegment2.getY(x);
		float verticalDistance = y1 - y2;
		float minFallDistance = getFallDistance(WALK_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy(), verticalDistance);
		float distance = (area2.getSide(orientation) - x) * orientation;
		NavigationEdgeType::Enum type;
		DirectedSegment fallArea = DirectedSegment(Point::Zero, Vector::Zero);

		if(distance < minFallDistance)
		{
			type = NavigationEdgeType::DESCEND;
			fallArea = DirectedSegment(x + orientation, y1, x, y2);
		}
		else
		{
			type = NavigationEdgeType::FALL;
			fallArea = DirectedSegment(x + orientation, y1, x + orientation, y2);
		}
		
		if(!intersectWithPlatform(fallArea, platforms))
		{
			node1.addEdge(new NavigationEdge(node1, node2, x, orientation, type));

			// BRODER
			float maxJumpHeight = getMaxJumpHeight(ANGLE_90_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy()) + 80.0f;
			if(verticalDistance <= maxJumpHeight)
				node2.addEdge(new NavigationEdge(node2, node1, x, Side::getOpposite(orientation), NavigationEdgeType::JUMP_UP));
		}
	}

	void NavigationGraph::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, ShapeCollection& platforms)
	{
		const YABP& area1 = node1.getArea();
		const YABP& area2 = node2.getArea();
		float horizontalDistance = area2.getLeft() - area1.getRight();

		float y1low = area1.getCenterY() + area1.getSlopedRadiusVy() - area1.getYRadius();
		float y2low = area2.getCenterY() - area2.getSlopedRadiusVy() - area2.getYRadius();
		float y2high = area2.getCenterY() - area2.getSlopedRadiusVy() + area2.getYRadius();
		float maxJumpForwardDistance = getMaxJumpDistance(ANGLE_45_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy());
		if(y1low >= y2low && y1low <= y2high  && horizontalDistance <= maxJumpForwardDistance)
		{
			DirectedSegment jumpArea = DirectedSegment(area1.getRight() + 1.0f, y1low - 1.0f, area2.getLeft() - 1.0f, y2low + 1.0f);
			if(!intersectWithPlatform(jumpArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::JUMP_FORWARD));
				node2.addEdge(new NavigationEdge(node2, node1, area2.getLeft(), Side::LEFT, NavigationEdgeType::JUMP_FORWARD));
			}
		}
	}

	void NavigationGraph::createEdges(ShapeCollection& platforms)
	{
		// Create edges
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			NavigationNode& node1 = **i;
			const YABP& area1 = node1.getArea();
			for(NavigationNodeIterator j = _nodes.begin(); j != _nodes.end(); ++j)
			{
				NavigationNode& node2 = **j;
				if(i == j)
					continue;

				const YABP& area2 = node2.getArea();

				// Check walk
				if(intersects(area1, area2))
				{
					if(area1.getLeft() <= area2.getLeft())
					{
						node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::WALK));
						node2.addEdge(new NavigationEdge(node2, node1, area1.getRight(), Side::LEFT, NavigationEdgeType::WALK));
					}
				}
				// check fall/jump up
				// BRODER
				else if(area1.getBottom() > area2.getBottom() && area1.getLeft() -20.f <= area2.getRight() && area1.getRight() + 20.0f >= area2.getLeft())
				{
					if(area1.getLeft() >= area2.getLeft())
						checkVerticalEdges(node1, node2, area1.getLeft(), Side::LEFT, platforms);
					if(area1.getRight() <= area2.getRight())
						checkVerticalEdges(node1, node2, area1.getRight(), Side::RIGHT, platforms);
				}
				// check jump forward
				else if(area1.getRight() < area2.getLeft() && area1.getTop() == area2.getTop())
				{
					checkHorizontalEdges(node1, node2, platforms);
				}
			}
		}

		// Remove nodes without edges
		removeNodesWithoutEdges(_nodes);
	}

	void addPlatform(const StaticBody& body, ShapeCollection& platforms) 
	{
		if(!body.isCover())
		{
			const Shape& platform = body.getShape();
			platforms.push_back(&platform);
		}
	}

	void NavigationGraph::init(void)
	{
		ShapeCollection platforms;
		const EntityCollection& entities = EntitiesManager::get().getEntities();
		for(EntityIterator i = entities.begin(); i != entities.end(); ++i)
		{
			const Entity& entity = (*(*i).second);
			const Component* component = entity.get(ComponentType::STATIC_BODY);
			if(component != NULL)
				addPlatform((const StaticBody&)(*component), platforms);
		}
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
		}
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			const NavigationEdgeCollection& edges = node.getEdges();
			for(NavigationEdgeIterator j = edges.begin(); j != edges.end(); ++j)
			{
				const NavigationEdge& edge = **j;
				const YABP& area2 = edge.getTarget().getArea();
				float x1 = edge.getX();
				float x2 = (edge.getType() == NavigationEdgeType::JUMP_FORWARD) ? area2.getOppositeSide(edge.getSide()) : x1;
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
				Graphics::get().draw(Segment(Point(x1, y1), Point(x2, y2)), color);
			}
		}
	}
}