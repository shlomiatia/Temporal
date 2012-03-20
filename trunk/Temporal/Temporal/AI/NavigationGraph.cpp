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
		for(std::vector<const NavigationEdge*>::iterator i = _edges.begin(); i != _edges.end(); ++i)
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

	void cutAreaLeft(const Rect& platform, const Rect& area, std::vector<const Rect>& areas, std::vector<const Rect>::iterator& iterator)
	{
		float width = area.getRight() - platform.getRight() + 1.0f;
		const Rect nodeAfterCut = RectLB(platform.getRight(), area.getBottom(), width, area.getHeight());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreaRight(const Rect& platform, const Rect& area, std::vector<const Rect>& areas, std::vector<const Rect>::iterator& iterator)
	{
		float width = platform.getLeft() - area.getLeft() + 1.0f;
		const Rect nodeAfterCut = RectLB(area.getLeft(), area.getBottom(), width, area.getHeight());
		iterator = areas.insert(iterator, nodeAfterCut);
	}

	void cutAreasByPlatforms(std::vector<const Rect>& areas, std::vector<const Rect>& platforms)
	{
		for(unsigned int i = 0; i < platforms.size(); ++i)
		{
			const Rect& platform = platforms[i];
			for(std::vector<const Rect>::iterator k = areas.begin(); k != areas.end(); ++k)
			{	
				const Rect area = *k;
				if(area.intersectsExclusive(platform))
				{
					k = areas.erase(k);
					if(platform.getLeft() <= area.getLeft() && platform.getRight() <= area.getRight())
					{
						cutAreaLeft(platform, area, areas, k);
					}
					else if(platform.getLeft() >= area.getLeft() && platform.getRight() >= area.getRight())
					{
						cutAreaRight(platform, area, areas, k);
					}
					else if(platform.getLeft() >= area.getLeft() && platform.getRight() <= area.getRight())
					{
						cutAreaLeft(platform, area, areas, k);
						cutAreaRight(platform, area, areas, k);
					}
					else if(k == areas.end())
					{
						break;
					}
				}
			}
		}
	}

	bool intersectWithPlatform(const Rect& area, std::vector<const Rect>& platforms)
	{
		for(int unsigned i = 0; i < platforms.size(); ++i)
		{
			const Rect& platform = platforms[i];
			if(platform.intersectsExclusive(area))
			{
				return true;
			}
		}
		return false;
	}

	void NavigationGraph::createNodes(std::vector<const Rect>& platforms)
	{
		for(unsigned int i = 0; i < platforms.size(); ++i)
		{
			// Create area from platform
			const Rect& platform = platforms[i];
			if(platform.getWidth() < MIN_AREA_SIZE.getWidth())
				continue;

			const Rect area = RectLB(platform.getLeft() - MIN_FALL_DISTANCE, platform.getTop(), platform.getWidth() + 2 * MIN_FALL_DISTANCE, MIN_AREA_SIZE.getHeight());
			std::vector<const Rect> areas;
			areas.push_back(area);
			cutAreasByPlatforms(areas, platforms);

			// TODO: Handle areas intersection PHYISICS

			// TODO: Give up, use std... SLOTH
			// Create nodes from areas
			for(unsigned int j = 0; j < areas.size(); ++j)
			{
				const Rect& area = areas[j];

				// Check min width
				if(area.getWidth() >= MIN_AREA_SIZE.getWidth())
					_nodes.push_back(new NavigationNode(area));
			}
		}
	}

	void NavigationGraph::checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Orientation::Enum orientation, std::vector<const Rect>& platforms)
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

	void NavigationGraph::checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, std::vector<const Rect>& platforms)
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

	void NavigationGraph::createEdges(std::vector<const Rect>& platforms)
	{
		// Create edges
		for(unsigned int i = 0; i < _nodes.size(); ++i)
		{
			NavigationNode& node1 = *_nodes[i];
			const Rect& area1 = node1.getArea();
			for(unsigned int j = 0; j < _nodes.size(); ++j)
			{
				NavigationNode& node2 = *_nodes[j];
				const Rect& area2 = node2.getArea();
				// check fall/jump up
				if(area1.getTop() > area2.getTop() && area1.getLeft() <= area2.getRight() && area1.getRight() >= area2.getLeft())
				{
					if(area1.getLeft() >= area2.getLeft())
						checkVerticalEdges(node1, node2, area1.getLeft(), Orientation::LEFT, platforms);
					if(area1.getRight() <= area2.getRight())
						checkVerticalEdges(node1, node2, area1.getRight(), Orientation::RIGHT, platforms);
				}
				// check jump forward
				else if(area1.getRight() < area2.getLeft())
				{
					checkHorizontalEdges(node1, node2, platforms);
				}
			}
		}

		// Remove nodes without edges
		for(std::vector<NavigationNode*>::iterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			const NavigationNode& node = **i;
			if(node.getEdges().size() == 0)
				i = _nodes.erase(i);
		}
	}

	const NavigationNode* NavigationGraph::getNodeByPosition(const Vector& position) const
	{
		for(unsigned int i = 0; i < _nodes.size(); ++i)
		{
			const NavigationNode* node = _nodes[i];
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
			std::vector<const Rect>& platforms = *((std::vector<const Rect>*)data);
			Rect platform(Vector::Zero, Vector(1.0f, 1.0f));
			entity.handleMessage(Message(MessageID::GET_BOUNDS, &platform));
			platforms.push_back(platform);
		}
	}

	void NavigationGraph::init(void)
	{
		std::vector<const Rect> platforms;
		EntitiesManager::get().iterateEntities(ComponentType::STATIC_BODY, &platforms, &addPlatform);
		createNodes(platforms);
		createEdges(platforms);
	}

	void NavigationGraph::dispose(void)
	{
		for(std::vector<NavigationNode*>::iterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			delete *i;
		}
	}

	void NavigationGraph::draw(void) const
	{
		for(unsigned int i = 0; i < _nodes.size(); ++i)
		{
			const NavigationNode& node = *_nodes[i];
			Graphics::get().drawRect(node.getArea(), Color::Yellow);
			const std::vector<const NavigationEdge*>& edges = node.getEdges();
			for(unsigned int j = 0; j < edges.size(); ++j)
			{
				const NavigationEdge& edge = *edges[j];
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