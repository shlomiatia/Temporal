#include "NavigationGraphGenerator.h"
#include "NavigationNode.h"
#include "NavigationEdge.h"
#include "ShapeOperations.h"
#include "Math.h"
#include "ActionController.h"
#include "MovementUtils.h"
#include "DynamicBody.h"
#include "PhysicsEnums.h"
#include "Grid.h"

namespace Temporal
{
	// BRODER
	const Vector NavigationGraphGenerator::MIN_AREA_SIZE = Vector(32.0f, 94.0f);

	Vector getBottomRight(const OBB& obb)
	{
		if (obb.getAxisX().getX() >= 0.0f)
		{
			return obb.getCenter() + obb.getAxisX() * obb.getRadiusX() + obb.getAxisX().getRightNormal() * obb.getRadiusY();
		}
		else
		{
			return obb.getCenter() - obb.getAxisX() * obb.getRadiusX() + (-obb.getAxisX()).getRightNormal() * obb.getRadiusY();
		}
	}

	Vector getBottomLeft(const OBB& obb)
	{
		if (obb.getAxisX().getX() >= 0.0f)
		{
			return obb.getCenter() - obb.getAxisX() * obb.getRadiusX() + (-obb.getAxisX()).getLeftNormal() * obb.getRadiusY();
		}
		else
		{
			return obb.getCenter() + obb.getAxisX() * obb.getRadiusX() + obb.getAxisX().getLeftNormal() * obb.getRadiusY();
		}
	}

	void cutArea(Side::Enum direction, float cutAmount, const OBB& area, OBBList& areas, OBBIterator& iterator)
	{
		Axis::Enum axisSign = AngleUtils::radian().isModerate(area.getAxisX().getAngle()) ? Axis::X : Axis::Y;
		Vector normalizedSlopedVector = area.getAxis(axisSign);
		float length = cutAmount / normalizedSlopedVector.getX();
		Vector cutRadius = (normalizedSlopedVector * length) / 2.0f;

		// Move center in the opposite direction
		Vector center = area.getCenter() + static_cast<float>(Side::getOpposite(direction)) * cutRadius;
		Vector radius = area.getRadius();
		radius.setX(radius.getX() - cutAmount / 2.0f);

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

	Segment getLowerSegment(const OBB& obb)
	{
		return Segment(obb.getCenter() - obb.getAxisY() * obb.getRadiusY(), obb.getAxisX() * obb.getRadiusX());
	}

	bool intersectWithPlatform(const DirectedSegment& area, OBBList& platforms)
	{
		for (OBBIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const OBB& platform = *i;
			if (intersects(area, platform))
			{
				return true;
			}
		}
		return false;
	}

	void removeNodesWithoutEdges(NavigationNodeList& nodes)
	{
		for (NavigationNodeIterator i = nodes.begin(); i != nodes.end();)
		{
			const NavigationNode& inode = **i;
			bool haveEdge = inode.getEdges().size() > 0;

			// Check if any edge is directed to this node
			if (!haveEdge)
			{
				for (NavigationNodeIterator j = nodes.begin(); j != nodes.end(); ++j)
				{
					const NavigationNode& jnode = **j;
					for (NavigationEdgeIterator k = jnode.getEdges().begin(); k != jnode.getEdges().end(); ++k)
					{
						const NavigationEdge& edge = **k;
						if (&(edge.getTarget()) == &inode)
						{
							haveEdge = true;
							break;
						}
					}
					if (haveEdge)
						break;
				}
			}

			if (!haveEdge)
			{
				delete *i;
				i = nodes.erase(i);
			}
			else
				++i;
		}
	}

	void NavigationGraphGenerator::cutAreasByPlatforms(OBBList& areas, OBBList& platforms)
	{
		for (OBBIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			const OBB& platform = *i;

			for (OBBIterator j = areas.begin(); j != areas.end(); ++j)
			{
				OBB area = *j;

				if (intersects(area, platform))
				{
					j = areas.erase(j);

					Vector startLeft = getBottomLeft(area);
					Vector startRight = getBottomRight(area);
					Axis::Enum axisX = AngleUtils::radian().isModerate(area.getAxisX().getAngle()) ? Axis::X : Axis::Y;
					Vector vectorX = area.getAxis(axisX);
					Vector vectorLeft = vectorX.getX() >= 0.0f ? vectorX : -vectorX;
					Vector vectorRight = vectorX.getX() < 0.0f ? vectorX : -vectorX;

					RayCastResult result;
					_grid.cast(startLeft, vectorLeft, result, CollisionCategory::OBSTACLE);
					if (result.getDirectedSegment().getTarget() != Vector::Zero && result.getFixture().getGlobalShape() == platform)
					{
						float amount = area.getRadius().getAxis(axisX) * 2.0f - (result.getDirectedSegment().getTarget() - startLeft).getLength() + 1.0f;
						if (amount > 0.0f)
							cutArea(Side::RIGHT, amount, area, areas, j);
					}
					else
					{
						float min = platform.getLeft();
						float left = area.getLeft();
						float right = area.getRight();
						if (min >= left && min <= right)
							cutAreaRight(min - 1.0f, area, areas, j);
					}
					result = RayCastResult();
					_grid.cast(startRight, vectorRight, result, CollisionCategory::OBSTACLE);
					if (result.getDirectedSegment().getTarget() != Vector::Zero && result.getFixture().getGlobalShape() == platform)
					{
						float amount = area.getRadius().getAxis(axisX) * 2.0f - (result.getDirectedSegment().getTarget() - startRight).getLength() + 1.0f;
						if (amount > 0.0f)
							cutArea(Side::LEFT, amount, area, areas, j);
					}
					else
					{
						float max = platform.getRight();
						float left = area.getLeft();
						float right = area.getRight();
						if (max >= left && max <= right)
							cutAreaLeft(max + 1.0f, area, areas, j);
					}

					if (j == areas.end())
					{
						break;
					}
				}
			}
		}
	}

	void NavigationGraphGenerator::createNodes(OBBList& platforms)
	{
		OBBList areas;
		for (OBBIterator i = platforms.begin(); i != platforms.end(); ++i)
		{
			// Create area from platform
			const OBB& platform = *i;
			Vector axisX = platform.getAxisX();
			float axisXAngle = axisX.getAngle();
			Axis::Enum platformAxis = AngleUtils::radian().isModerate(axisXAngle) ? Axis::X : Axis::Y;
			Axis::Enum platformOppositeAxisSign = Axis::getOpposite(platformAxis);
			Vector platformOppositeAxis = platform.getAxis(platformOppositeAxisSign);
			if (platformOppositeAxis.getY() < 0.0f)
			{
				platformOppositeAxis = -platformOppositeAxis;
			}
			Vector center = platform.getCenter() + platformOppositeAxis * (platform.getRadius().getAxis(platformOppositeAxisSign) + MIN_AREA_SIZE.getY() / 2.0f + 1.0f);
			Vector radius = Vector::Zero;
			radius.setAxis(platformAxis, platform.getRadius().getAxis(platformAxis));
			radius.setAxis(platformOppositeAxisSign, MIN_AREA_SIZE.getY() / 2.0f);
			OBB area = OBB(center, axisX, radius);

			OBBList platformAreas;
			platformAreas.push_back(area);
			cutAreasByPlatforms(platformAreas, platforms);

			// Create nodes from areas
			for (OBBIterator j = platformAreas.begin(); j != platformAreas.end(); ++j)
			{
				const OBB& area = *j;

				Vector axisVector = area.getAxis(platformAxis) * area.getRadius().getAxis(platformAxis);
				Vector oppositeAxisVector = area.getAxis(platformOppositeAxisSign) * area.getRadius().getAxis(platformOppositeAxisSign);
				// Check min width
				if (axisVector.getLength() * 2.0f >= MIN_AREA_SIZE.getX() && oppositeAxisVector.getLength() * 2.0f >= MIN_AREA_SIZE.getY())
				{
					areas.push_back(area);
				}
			}
		}

		for (OBBIterator i = areas.begin(); i != areas.end(); ++i)
		{
			const OBB& area = *i;
			_nodes.push_back(new NavigationNode(area));
		}
	}

	void NavigationGraphGenerator::checkFallVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Side::Enum orientation, OBBList& platforms)
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

		if (verticalDistance > 1.0f && distance >= minFallDistance)
		{
			DirectedSegment fallArea = DirectedSegment(x + orientation, y1, x + orientation, y2);
			if (!intersectWithPlatform(fallArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, x, orientation, NavigationEdgeType::FALL));
			}
		}
	}

	void NavigationGraphGenerator::checkClimbDescendVerticalEdges(NavigationNode& node1, NavigationNode& node2, OBBList& platforms)
	{
		float x;
		if (node1.getArea().getLeft() < node2.getArea().getLeft())
		{
			if (node1.getArea().getRight() < node2.getArea().getRight())
				x = (node1.getArea().getRight() + node2.getArea().getLeft()) / 2.0f;
			else
				x = (node2.getArea().getRight() + node2.getArea().getLeft()) / 2.0f;
		}
		else
		{
			if (node1.getArea().getRight() > node2.getArea().getRight())
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

		if (!intersectWithPlatform(fallArea, platforms))
		{
			node1.addEdge(new NavigationEdge(node1, node2, x, static_cast<Side::Enum>(0), NavigationEdgeType::DESCEND));

			// BRODER
			float maxJumpHeight = getMaxJumpHeight(AngleUtils::radian().ANGLE_90_IN_RADIANS, ActionController::JUMP_FORCE_PER_SECOND, DynamicBody::GRAVITY.getY()) + 80.0f;
			if (verticalDistance <= maxJumpHeight)
				node2.addEdge(new NavigationEdge(node2, node1, x, static_cast<Side::Enum>(0), NavigationEdgeType::JUMP_UP));
		}
	}

	void NavigationGraphGenerator::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, OBBList& platforms)
	{
		const OBB& area1 = node1.getArea();
		const OBB& area2 = node2.getArea();
		float horizontalDistance = area2.getLeft() - area1.getRight();

		float maxJumpForwardDistance = getMaxJumpDistance(AngleUtils::radian().ANGLE_45_IN_RADIANS, ActionController::JUMP_FORCE_PER_SECOND + 125.0f - fabsf(area1.getBottom() - area2.getBottom()), DynamicBody::GRAVITY.getY());
		if (horizontalDistance <= maxJumpForwardDistance)
		{
			float minBottom = fminf(area1.getBottom(), area2.getBottom());
			float maxTop = fmaxf(area1.getTop(), area2.getTop());
			DirectedSegment jumpArea = DirectedSegment(area1.getRight() + 1.0f, minBottom - 1.0f, area2.getLeft() - 1.0f, maxTop + 1.0f);
			if (jumpArea.getVector().getX() < 0.0f)
			{
				if (node1.getArea().getBottom() < node2.getArea().getBottom())
					node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::JUMP_FORWARD));
				if (node1.getArea().getBottom() >= node2.getArea().getBottom())
					node2.addEdge(new NavigationEdge(node2, node1, area2.getLeft(), Side::LEFT, NavigationEdgeType::JUMP_FORWARD));
			}
			else if (!intersectWithPlatform(jumpArea, platforms))
			{
				node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::JUMP_FORWARD));
				node2.addEdge(new NavigationEdge(node2, node1, area2.getLeft(), Side::LEFT, NavigationEdgeType::JUMP_FORWARD));
			}
		}
	}

	void NavigationGraphGenerator::createEdges(OBBList& platforms)
	{
		// Create edges
		for (NavigationNodeIterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			NavigationNode& node1 = **i;
			const OBB& area1 = node1.getArea();
			for (NavigationNodeIterator j = _nodes.begin(); j != _nodes.end(); ++j)
			{
				NavigationNode& node2 = **j;
				if (i == j)
					continue;

				const OBB& area2 = node2.getArea();

				if ((getBottomLeft(area1) - getBottomRight(area2)).getLength() < 10.0f || (getBottomLeft(area2) - getBottomRight(area1)).getLength() < 10.0f)
				{
					if (area1.getLeft() <= area2.getLeft())
					{
						node1.addEdge(new NavigationEdge(node1, node2, area1.getRight(), Side::RIGHT, NavigationEdgeType::WALK));
						node2.addEdge(new NavigationEdge(node2, node1, area1.getRight(), Side::LEFT, NavigationEdgeType::WALK));
					}
				}
				else if (area1.getBottom() > area2.getBottom() && area1.getLeft() < area2.getRight() && area1.getRight() > area2.getLeft())
				{
					// check jump up/descend
					checkClimbDescendVerticalEdges(node1, node2, platforms);

				}
				// check jump forward
				else if (area1.getRight() <= area2.getLeft() && fabsf(area1.getTop() - area2.getTop()) <= 96.0f)
				{
					checkHorizontalEdges(node1, node2, platforms);
				}

				// BRODER
				if (area1.getBottom() > area2.getBottom() && area1.getLeft() - 20.f < area2.getRight() && area1.getRight() + 20.0f > area2.getLeft())
				{
					if (area1.getLeft() >= area2.getLeft())
						checkFallVerticalEdges(node1, node2, area1.getLeft(), Side::LEFT, platforms);
					if (area1.getRight() <= area2.getRight())
						checkFallVerticalEdges(node1, node2, area1.getRight(), Side::RIGHT, platforms);
				}
			}
		}

		// Remove nodes without edges
		//removeNodesWithoutEdges(_nodes);
	}

	NavigationGraphGenerator::NavigationGraphGenerator(const Grid& grid, OBBList& platforms)
		: _grid(grid)
	{
		createNodes(platforms);
		createEdges(platforms);
	}
}