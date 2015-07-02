#include "NavigationGraph.h"
#include "Shapes.h"
#include "ShapeOperations.h"
#include "Math.h"
#include "EntitySystem.h"
#include "ActionController.h"
#include "MovementUtils.h"
#include "DynamicBody.h"
#include "StaticBody.h"
#include "Fixture.h"
#include "Graphics.h"
#include "PhysicsEnums.h"
#include "CollisionFilter.h"
#include "Grid.h"

namespace Temporal
{
	// BRODER
	const Vector NavigationGraph::MIN_AREA_SIZE = Vector(32.0f, 96.0f);

	NavigationNode::~NavigationNode()
	{
		for(NavigationEdgeIterator i = _edges.begin(); i != _edges.end(); ++i)
		{
			delete *i;
		}
	}

	float NavigationEdge::calculateCost(const NavigationNode& source)
	{
		const OBB& sourceArea = source.getArea();
		const OBB& targetArea = getTarget().getArea();

		Segment segment(sourceArea.getCenter(), targetArea.getCenter());
		return segment.getLength();
	}

	void cutArea(Side::Enum direction, float cutAmount, const OBB& area, OBBList& areas, OBBIterator& iterator)
	{
		Axis::Enum axisSign = AngleUtils::radian().isModerate(area.getAxisX().getAngle()) ? Axis::X : Axis::Y;
		Vector normalizedSlopedVector = area.getAxis(axisSign);
		float length = cutAmount / normalizedSlopedVector.getX();
		Vector cutRadius = (normalizedSlopedVector * length) / 2.0f;

		// Move center in the opposite direction
		Vector center = area.getCenter() + static_cast<float>(Side::getOpposite(direction)) * cutRadius;
		Vector radius = area.getRadius() - cutRadius;
		
		if (radius.getX() > 0.0f && radius.getY() > 0.0f)
		{
			const OBB nodeAfterCut = OBB(center, area.getAxisX(), radius);
			iterator = areas.insert(iterator, nodeAfterCut);
		}
	}

	void cutAreaLeft(float x, const OBB& area, OBBList& areas, OBBIterator& iterator)
	{
		float amount = x - area.getLeft();
		cutArea(Side::LEFT, amount, area, areas, iterator);
	}

	void cutAreaRight(float x, const OBB& area, OBBList& areas, OBBIterator& iterator)
	{
		float amount = area.getRight() - x;
		cutArea(Side::RIGHT, amount, area, areas, iterator);
	}

	// TODO: Do according to angle
	Segment getLowerSegment(const OBB& obb)
	{
		return Segment(obb.getCenter() - obb.getAxisY() * obb.getRadiusY(), obb.getAxisX() * obb.getRadiusX());
	}

	// TODO: Ray cast from each area vertice inward. Take min length from each side and cut
	void NavigationGraph::cutAreasByPlatforms(OBBList& areas, ShapeList& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const OBB& platform = **i;

			for(OBBIterator j = areas.begin(); j != areas.end(); ++j)
			{	
				OBB area = *j;
				
				Vector correction;

				// BRODER
				if(intersects(area, platform, &correction) && correction.getLength() > 20.0f)
				{
					j = areas.erase(j);
					
					float min = platform.getLeft();
					float max = platform.getRight();
					float left = area.getLeft();
					float right = area.getRight();
					if(max >= left && max <= right)
						cutAreaLeft(max + 1.0f, area, areas, j);
					if(min >= left && min <= right)
						cutAreaRight(min - 1.0f, area, areas, j);
					if(j == areas.end())
					{
						break;
					}
				}
			}
		}
	}

	bool intersectWithPlatform(const DirectedSegment& area, ShapeList& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const OBB& platform = **i;
			if(intersects(area, platform))
			{
				return true;
			}
		}
		return false;
	}

	void removeNodesWithoutEdges(NavigationNodeList& nodes)
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

	const NavigationNode* NavigationGraph::getNode(const OBB& shape) const
	{
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode* node = *i;
			if(intersects(node->getArea(),  shape))
				return node;
		}
		return 0;
	}

	void NavigationGraph::createNodes(ShapeList& platforms)
	{
		for(ShapeIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			// Create area from platform
			const OBB& platform = **i;
			Vector axisX = platform.getAxisX();
			float axisXAngle = axisX.getAngle();
			Axis::Enum platformAxis = AngleUtils::radian().isModerate(axisXAngle) ? Axis::X : Axis::Y;
			Axis::Enum platformOppositeAxisSign = Axis::getOpposite(platformAxis);
			Vector platformOppositeAxis = platform.getAxis(platformOppositeAxisSign);
			if(platformOppositeAxis.getY() < 0.0f)
			{
				platformOppositeAxis = -platformOppositeAxis;
			}
			Vector center = platform.getCenter() +  platformOppositeAxis * (platform.getRadius().getAxis(platformOppositeAxisSign) + MIN_AREA_SIZE.getY() / 2.0f + 1.0f);
			Vector radius = Vector::Zero;
			radius.setAxis(platformAxis, platform.getRadius().getAxis(platformAxis));
			radius.setAxis(platformOppositeAxisSign, MIN_AREA_SIZE.getY() / 2.0f);
			OBB area = OBB(center, axisX, radius);

			OBBList areas;
			areas.push_back(area);
			cutAreasByPlatforms(areas, platforms);
			
			// Create nodes from areas
			for(OBBIterator j = areas.begin(); j != areas.end(); ++j)
			{
				const OBB& area = *j;

				Vector axisVector = area.getAxis(platformAxis) * area.getRadius().getAxis(platformAxis);
				Vector oppositeAxisVector = area.getAxis(platformOppositeAxisSign) * area.getRadius().getAxis(platformOppositeAxisSign);
				// Check min width
				if(axisVector.getLength() * 2.0f >= MIN_AREA_SIZE.getX() && oppositeAxisVector.getLength() * 2.0f >= MIN_AREA_SIZE.getY())
					_nodes.push_back(new NavigationNode(area));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Side::Enum orientation, ShapeList& platforms)
	{
		const OBB& area1 = node1.getArea();
		const OBB& area2 = node2.getArea();
		Segment lowerSegment1 = getLowerSegment(area1);
		Segment lowerSegment2 = getLowerSegment(area2);
		float y1 = lowerSegment1.getY(x);
		float y2 = lowerSegment2.getY(x);
		float verticalDistance = y1 - y2;
		//BRODER
		float minFallDistance = getFallDistance(125.0f, DynamicBody::GRAVITY.getY(), verticalDistance);
		float distance = (area2.getSide(orientation) - x) * orientation;

		if(distance >= minFallDistance)
		{
			DirectedSegment fallArea = DirectedSegment(x + orientation, y1, x + orientation, y2);
			if(!intersectWithPlatform(fallArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, x, orientation, NavigationEdgeType::FALL));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, ShapeList& platforms)
	{
		float x;
		if(node1.getArea().getLeft() < node2.getArea().getLeft())
		{
			if(node1.getArea().getRight() < node2.getArea().getRight())
				x = (node1.getArea().getRight() + node2.getArea().getLeft()) / 2.0f;
			else
				x = (node2.getArea().getRight() + node2.getArea().getLeft()) / 2.0f;
		}
		else
		{
			if(node1.getArea().getRight() > node2.getArea().getRight())	
				x = (node1.getArea().getLeft() + node2.getArea().getRight()) / 2.0f;
			else
				x = (node1.getArea().getRight() + node1.getArea().getLeft()) / 2.0f;
		}
		const OBB& area1 = node1.getArea();
		const OBB& area2 = node2.getArea();
		Segment lowerSegment1 = getLowerSegment(area1);
		Segment lowerSegment2 = getLowerSegment(area2);
		float y1 = lowerSegment1.getY(x);
		float y2 = lowerSegment2.getY(x);
		float verticalDistance = y1 - y2;

		DirectedSegment fallArea = DirectedSegment(x, y1 - 2.0f, x, y2);
		
		if(!intersectWithPlatform(fallArea, platforms))
		{
			node1.addEdge(new NavigationEdge(node1, node2, x, Side::LEFT, NavigationEdgeType::DESCEND));
			
			// BRODER
			float maxJumpHeight = getMaxJumpHeight(AngleUtils::radian().ANGLE_90_IN_RADIANS, ActionController::JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getY()) + 80.0f;
			if(verticalDistance <= maxJumpHeight)
				node2.addEdge(new NavigationEdge(node2, node1, x, Side::LEFT, NavigationEdgeType::JUMP_UP));
		}
	}

	void NavigationGraph::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, ShapeList& platforms)
	{
		const OBB& area1 = node1.getArea();
		const OBB& area2 = node2.getArea();
		float horizontalDistance = area2.getLeft() - area1.getRight();

		float maxJumpForwardDistance = getMaxJumpDistance(AngleUtils::radian().ANGLE_45_IN_RADIANS, ActionController::JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getY());
		if(horizontalDistance <= maxJumpForwardDistance)
		{
			DirectedSegment jumpArea = DirectedSegment(area1.getRight() + 1.0f, area1.getBottom() - 1.0f, area2.getLeft() - 1.0f, area1.getTop() + 1.0f);
			if(!intersectWithPlatform(jumpArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::JUMP_FORWARD));
				node2.addEdge(new NavigationEdge(node2, node1, area2.getLeft(), Side::LEFT, NavigationEdgeType::JUMP_FORWARD));
			}
		}
	}

	void NavigationGraph::createEdges(ShapeList& platforms)
	{
		// Create edges
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			NavigationNode& node1 = **i;
			const OBB& area1 = node1.getArea();
			for(NavigationNodeIterator j = _nodes.begin(); j != _nodes.end(); ++j)
			{
				NavigationNode& node2 = **j;
				if(i == j)
					continue;

				const OBB& area2 = node2.getArea();

				// Check walk
				if(intersects(area1, area2))
				{
					if(area1.getLeft() <= area2.getLeft())
					{
						node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::WALK));
						node2.addEdge(new NavigationEdge(node2, node1, area1.getRight(), Side::LEFT, NavigationEdgeType::WALK));
					}
				}
				// BRODER
				else if(area1.getBottom() > area2.getBottom() && area1.getLeft() -20.f < area2.getRight() && area1.getRight() + 20.0f > area2.getLeft())
				{
					// check jump up/descend
					if(area1.getBottom() > area2.getBottom() && area1.getLeft() < area2.getRight() && area1.getRight() > area2.getLeft())
						checkVerticalEdges(node1, node2, platforms);

					// check fall
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

	void addPlatform(const StaticBody& body, ShapeList& platforms) 
	{
		if(body.getFixture().getCategory() == CollisionCategory::OBSTACLE)
		{
			const OBB& platform = body.getFixture().getGlobalShape();
			platforms.push_back(&platform);
		}
	}

	void NavigationGraph::init(GameState* gameState)
	{
		GameStateComponent::init(gameState);
		ShapeList platforms;
		const HashEntityMap& entities = getGameState().getEntitiesManager().getEntities();
		for(HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
		{
			const Entity& entity = *(i->second);
			const Component* component = entity.get(StaticBody::TYPE);
			if(component)
				addPlatform(*static_cast<const StaticBody*>(component), platforms);
		}
		createNodes(platforms);
		createEdges(platforms);
	}

	NavigationGraph::~NavigationGraph()
	{
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			delete *i;
		}
	}

	void NavigationGraph::draw() const
	{
		Graphics::get().getLinesSpriteBatch().begin();
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			Graphics::get().getLinesSpriteBatch().add(node.getArea(), Color::Yellow);
		}
		for(NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
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