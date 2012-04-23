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
		const SlopedArea& sourceArea = source.getArea();
		const SlopedArea& targetArea = getTarget().getArea();

		Segment segment(sourceArea.getCenter(), targetArea.getCenter());
		return segment.getLength();
	}

	void cutAreaLeft(float x, const SlopedArea& area, SlopedAreaCollection& areas, SlopedAreaIterator& iterator)
	{
		Vector normalizedSlopedVector = area.getSlopedRadius().normalize();
		float l = (x - area.getLeft()) / normalizedSlopedVector.getVx();
		Vector d = (normalizedSlopedVector * l) / 2.0f;
		Point center = area.getCenter() + d;
		Vector slopedRadius = area.getSlopedRadius() - d;
		const SlopedArea nodeAfterCut = SlopedArea(center, slopedRadius, area.getYRadius());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreaRight(float x, const SlopedArea& area, SlopedAreaCollection& areas, SlopedAreaIterator& iterator)
	{
		Vector normalizedSlopedVector = area.getSlopedRadius().normalize();
		float l = (area.getRight() - x) / normalizedSlopedVector.getVx();
		Vector d = (normalizedSlopedVector * l) / 2.0f;
		Point center = area.getCenter() - d;
		Vector slopedRadius = area.getSlopedRadius() - d;
		const SlopedArea nodeAfterCut = SlopedArea(center, slopedRadius, area.getYRadius());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	bool intersects(const SlopedArea& slopedArea, const Segment& segment)
	{
		Vector segRadius = segment.getRadius();
		float delta = slopedArea.getCenterX() - segment.getCenterX();
		if(slopedArea.getSlopedRadiusVx() + segRadius.getVx() < abs(delta)) return false;

		// TODO: Axis
		Vector normal = slopedArea.getSlopedRadius().normalize().getLeftNormal();
		Vector yRadius = Vector(0.0f, slopedArea.getYRadius());
		Point slopedAreaPointMin = slopedArea.getCenter() + yRadius;
		Point slopedAreaPointMax = slopedArea.getCenter() - yRadius;
		float slopedAreaProjectionMin =   normal * slopedAreaPointMin;
		float slopedAreaProjectionMax =   normal * slopedAreaPointMax;
		float segmentProjection1 = normal * segment.getPoint1();
		float segmentProjection2 = normal * segment.getPoint2();
		if((segmentProjection1 < slopedAreaProjectionMin && segmentProjection2 < slopedAreaProjectionMin) ||
		   (segmentProjection1 > slopedAreaProjectionMax && segmentProjection2 > slopedAreaProjectionMax))
		   return false;

		normal = segRadius.normalize().getLeftNormal();
		float point = normal * segment.getCenter();
		Vector yabpRadius = slopedArea.getSlopedRadius() + yRadius;
		Vector absNormal = Vector(abs(normal.getVx()), abs(normal.getVy()));
		Vector absSlopedRadius = Vector(abs(slopedArea.getSlopedRadius().getVx()), abs(slopedArea.getSlopedRadius().getVy()));
		float max = normal * slopedArea.getCenter()  + absSlopedRadius * absNormal + yRadius * absNormal;
		float min = normal * slopedArea.getCenter()  - absSlopedRadius * absNormal - yRadius * absNormal;
		if(point < min || point > max) return false;
		return true;
	}
	
	bool slopeAxisOverlapps(const SlopedArea& slopedArea1, const SlopedArea& slopedArea2)
	{
		Vector normal = slopedArea1.getSlopedRadius().normalize().getLeftNormal();
		Vector yRadius1 = Vector(0.0f, slopedArea1.getYRadius());
		Point yabpMinPoint1 = slopedArea1.getCenter() + yRadius1;
		Point yabpMaxPoint1 = slopedArea1.getCenter() - yRadius1;
		float yabpMinProjection1 =   normal * yabpMinPoint1;
		float yabpMaxProjection1 =   normal * yabpMaxPoint1;
		Vector absNormal = Vector(abs(normal.getVx()), abs(normal.getVy()));
		Vector yRadius2 = Vector(0.0f, slopedArea2.getYRadius());
		Vector yabpRadius2 = slopedArea2.getSlopedRadius() + yRadius2;
		Vector absSlopedRadius2 = Vector(abs(slopedArea2.getSlopedRadius().getVx()), abs(slopedArea2.getSlopedRadius().getVy()));
		float yabpMinProjection2 = normal * slopedArea2.getCenter() - absSlopedRadius2 * absNormal - yRadius2 * absNormal;
		float yabpMaxProjection2 = normal * slopedArea2.getCenter()  + absSlopedRadius2 * absNormal + yRadius2 * absNormal;
		
		return !((yabpMinProjection2 < yabpMinProjection1 && yabpMaxProjection2 < yabpMinProjection1) ||
				 (yabpMinProjection2 > yabpMaxProjection1 && yabpMaxProjection2 > yabpMaxProjection1));
	}

	bool intersects(const SlopedArea& slopedArea1, const SlopedArea& slopedArea2)
	{
		float delta = slopedArea1.getCenterX() - slopedArea2.getCenterX();
		if(slopedArea1.getSlopedRadiusVx() + slopedArea2.getSlopedRadiusVx() < abs(delta)) return false;

		if(!slopeAxisOverlapps(slopedArea1, slopedArea2))
			return false;

		if(!slopeAxisOverlapps(slopedArea2, slopedArea1))
			return false;

		return true;
	}

	bool intersects(const SlopedArea& slopedArea, const AABB& aabb)
	{
		float delta = slopedArea.getCenterX() - aabb.getCenterX();
		if(slopedArea.getSlopedRadiusVx() + aabb.getRadiusVx()  < abs(delta)) return false;
		delta = slopedArea.getCenterY() - aabb.getCenterY();
		if(slopedArea.getSlopedRadiusVy() + slopedArea.getYRadius() + aabb.getRadiusVy()  < abs(delta)) return false;

		// TODO: Axis
		Vector normal = slopedArea.getSlopedRadius().normalize().getLeftNormal();
		Vector yRadius = Vector(0.0f, slopedArea.getYRadius());
		Point slopedAreaPointMin = slopedArea.getCenter() + yRadius;
		Point slopedAreaPointMax = slopedArea.getCenter() - yRadius;
		float slopedAreaProjectionMin =   normal * slopedAreaPointMin;
		float slopedAreaProjectionMax =   normal * slopedAreaPointMax;
		Vector absNormal = Vector(abs(normal.getVx()), abs(normal.getVy()));
		float aabbProjectionMin = normal * aabb.getCenter() - absNormal * aabb.getRadius();
		float aabbProjectionMax = normal * aabb.getCenter() + absNormal * aabb.getRadius();
		if((aabbProjectionMin < slopedAreaProjectionMin && aabbProjectionMax < slopedAreaProjectionMin) ||
		   (aabbProjectionMin > slopedAreaProjectionMax && aabbProjectionMax > slopedAreaProjectionMax))
		   return false;
		return true;
	}

	void cutAreasByPlatforms(SlopedAreaCollection& areas, ShapeCollection& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const Shape& platform = **i;
			const Segment& segment = (const Segment&)platform;
			for(SlopedAreaIterator j = areas.begin(); j != areas.end(); ++j)
			{	
				const SlopedArea area = *j;
				if(intersects(area, segment) && area.getLeft() != segment.getRight() && area.getRight() != segment.getLeft() && area.getBottom() != segment.getBottom() && area.getTop() != segment.getBottom())
				{
					j = areas.erase(j);
					DirectedSegment upperSlope = DirectedSegment(area.getCenter() + Vector(0.0f, area.getYRadius()) - area.getSlopedRadius(), area.getCenter() + Vector(0.0f, area.getYRadius()) + area.getSlopedRadius());
					DirectedSegment lowerSlope = DirectedSegment(area.getCenter() - Vector(0.0f, area.getYRadius()) - area.getSlopedRadius(), area.getCenter() - Vector(0.0f, area.getYRadius()) + area.getSlopedRadius());
					Point upperSlopePoint = Point::Zero;
					Point lowerSlopePoint = Point::Zero;
					intersects(upperSlope, segment, &upperSlopePoint);
					intersects(lowerSlope, segment, &lowerSlopePoint);
					float min = segment.getLeft();
					float max = segment.getRight();
					if(upperSlopePoint != Vector::Zero)
						if(segment.getNaturalVector().getVy() > 0.0f || (segment.getNaturalVector().getVy() == 0.0f && area.getSlopedRadius().getVy() < 0.0f))
							max = upperSlopePoint.getX();
						else
							min = upperSlopePoint.getX();		
					if(lowerSlopePoint != Vector::Zero)
						if(segment.getNaturalVector().getVy() > 0.0f || (segment.getNaturalVector().getVy() == 0.0f && area.getSlopedRadius().getVy() < 0.0f))
							min = lowerSlopePoint.getX();
						else
							max = lowerSlopePoint.getX();
					if(max > area.getLeft() && max < area.getRight())
						cutAreaLeft(max + 1.0f, area, areas, j);
					if(min > area.getLeft() && min < area.getRight())
						cutAreaRight(min - 1.0f, area, areas, j);
					if(j == areas.end())
					{
						break;
					}
				}
			}
		}
	}

	bool intersectWithPlatform(const Segment& area, ShapeCollection& platforms)
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
			const Segment& segment = (const Segment&)platform;
			Vector vector = segment.getNaturalVector();
			float angle = vector.getAngle();
			if(!isModerateAngle(angle))
				continue;

			// Create area
			Point center = Point(segment.getCenterX(), segment.getCenterY() + 1.0f + MIN_AREA_SIZE.getHeight() / 2.0f);
			Vector slopedRadius = vector / 2.0f;
			float yRadius = MIN_AREA_SIZE.getHeight() / 2.0f;
			SlopedArea area = SlopedArea(center, slopedRadius, yRadius);

			SlopedAreaCollection areas;
			areas.push_back(area);
			cutAreasByPlatforms(areas, platforms);
			
			// Create nodes from areas
			for(SlopedAreaIterator j = areas.begin(); j != areas.end(); ++j)
			{
				const SlopedArea& area = *j;

				// Check min width
				if(area.getWidth() >= MIN_AREA_SIZE.getWidth())
					_nodes.push_back(new NavigationNode(area));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Orientation::Enum orientation, ShapeCollection& platforms)
	{
		const SlopedArea& area1 = node1.getArea();
		const SlopedArea& area2 = node2.getArea();

		Vector n1 = area1.getSlopedRadius().normalize();
		float l1 = (x - area1.getCenterX()) / n1.getVx();
		float y1 = area1.getCenterY() + n1.getVy() * l1;
		Vector n2 = area2.getSlopedRadius().normalize();
		float l2 = (x - area2.getCenterX()) / n2.getVx();
		float y2 = area2.getCenterY() + n2.getVy() * l2 - area2.getYRadius();
		const Segment fallArea = Segment(x, y1, x + orientation, y2);
		float verticalDistance = y2 - y1;
		if(!intersectWithPlatform(fallArea, platforms))
		{
			float distance = (area2.getSide(orientation) - x) * orientation;
			float minFallDistance = getFallDistance(WALK_FORCE_PER_SECOND, -DynamicBody::GRAVITY.getVy(), verticalDistance);

			// BRODER
			float maxJumpHeight = getMaxJumpHeight(ANGLE_90_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy()) + 80.0f;
			NavigationEdgeType::Enum type = distance < minFallDistance ? NavigationEdgeType::DESCEND : NavigationEdgeType::FALL;
			node1.addEdge(new NavigationEdge(node1, node2, 
				x, orientation, type));
			if(verticalDistance <= maxJumpHeight)
				node2.addEdge(new NavigationEdge(node2, node1, x, Orientation::getOpposite(orientation), NavigationEdgeType::JUMP));
		}
	}

	void NavigationGraph::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, ShapeCollection& platforms)
	{
		const SlopedArea& area1 = node1.getArea();
		const SlopedArea& area2 = node2.getArea();
		float horizontalDistance = area2.getLeft() - area1.getRight();

		float y1 = area1.getCenterY() + area1.getSlopedRadiusVy() - area1.getYRadius();
		float y2low = area2.getCenterY() - area2.getSlopedRadiusVy() - area2.getYRadius();
		float y2high = area2.getCenterY() - area2.getSlopedRadiusVy() + area2.getYRadius();
		float maxJumpForwardDistance = getMaxJumpDistance(ANGLE_45_IN_RADIANS, JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getVy());
		if(y1 >= y2low && y1 <= y2high  && horizontalDistance <= maxJumpForwardDistance)
		{
			Segment jumpArea = Segment(area1.getRight() + 1.0f, y1 - 1.0f, area2.getLeft() - 1.0f, y2low - 1.0f);
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
			const SlopedArea& area1 = node1.getArea();
			for(NavigationNodeIterator j = _nodes.begin(); j != _nodes.end(); ++j)
			{
				NavigationNode& node2 = **j;
				if(i == j)
					continue;

				const SlopedArea& area2 = node2.getArea();

				// Check walk
				if(intersects(area1, area2))
				{
					if(area1.getLeft() <= area2.getLeft())
					{
						node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Orientation::RIGHT, NavigationEdgeType::WALK));
						node2.addEdge(new NavigationEdge(node2, node1, area1.getRight(), Orientation::LEFT, NavigationEdgeType::WALK));
					}
				}
				// check fall/jump up
				// BRODER
				else if(area1.getTop() >= area2.getTop() && area1.getLeft() -20.f <= area2.getRight() && area1.getRight() + 20.0f >= area2.getLeft())
				{
					if(area1.getLeft() >= area2.getLeft())
						checkVerticalEdges(node1, node2, area1.getLeft(), Orientation::LEFT, platforms);
					if(area1.getRight() <= area2.getRight())
						checkVerticalEdges(node1, node2, area1.getRight(), Orientation::RIGHT, platforms);
				}
				// check jump forward
				else if(area1.getRight() < area2.getLeft() && area1.getTop() == area2.getTop())
				{
					checkHorizontalEdges(node1, node2, platforms);
				}
			}
		}

		// Remove nodes without edges
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end();)
		{
			const NavigationNode& node = **i;
			if(node.getEdges().size() == 0)
				i = _nodes.erase(i);
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
		}
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			const NavigationEdgeCollection& edges = node.getEdges();
			for(NavigationEdgeIterator j = edges.begin(); j != edges.end(); ++j)
			{
				const NavigationEdge& edge = **j;
				const SlopedArea& area2 = edge.getTarget().getArea();
				float x1 = edge.getX();
				float x2 = edge.getType() == NavigationEdgeType::JUMP ? area2.getOppositeSide(edge.getOrientation()) : x1;
				float y1 = edge.getType() == NavigationEdgeType::WALK ? node.getArea().getCenterY() : node.getArea().getBottom();
				float y2 = edge.getType() == NavigationEdgeType::WALK ? area2.getCenterY() : area2.getBottom();
				Graphics::get().draw(Segment(Point(x1, y1), Point(x2, y2)), Color(1.0f, 0.5f, 0.0f));
			}
		}
	}
}