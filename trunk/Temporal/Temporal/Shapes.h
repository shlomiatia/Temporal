#ifndef SHAPES_H
#define SHAPES_H

#include "BaseEnums.h"
#include "NumericPair.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Shapes
	 *********************************************************************************************/
	namespace ShapeType
	{
		enum Enum
		{
			AABB,
			SEGMENT,
			YABP
		};
	}

	class Shape
	{
	public:
		virtual ~Shape() {}
		
		virtual ShapeType::Enum getType() const = 0;

		virtual float getBottom() const = 0;
		virtual float getLeft() const = 0;
		virtual float getTop() const = 0;
		virtual float getRight() const = 0;

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }
	};

	/**********************************************************************************************
	 * AABB
	 *********************************************************************************************/
	class AABB : public Shape
	{
	public:
		static const AABB Zero;

		AABB(float centerX, float centerY, float width, float height);
		AABB(const Point& center, const Size& size);
		AABB(const Point& center, const Vector& radius);

		ShapeType::Enum getType() const { return ShapeType::AABB; }

		const Point& getCenter() const { return _center; }
		const Vector& getRadius() const { return _radius; }

		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		float getRadiusVx() const { return getRadius().getVx(); }
		float getRadiusVy() const { return getRadius().getVy(); }

		float getBottom() const { return getCenterY() - getRadiusVy(); }
		float getLeft() const {	return getCenterX() - getRadiusVx(); }
		float getTop() const { return getCenterY() + getRadiusVy(); }
		float getRight() const { return getCenterX() + getRadiusVx(); }

		float getWidth() const { return getRadiusVx() * 2.0f; }
		float getHeight() const { return getRadiusVy() * 2.0f; }

		Range getAxis(Axis::Enum axis) const { return axis == Axis::X ? Range(getLeft(), getRight()) : Range(getBottom(), getTop()); }

		bool contains(const Point& point) const;

		bool operator==(const AABB& other) const { return ((getCenter() == other.getCenter()) && (getRadius() == other.getRadius())); }
		bool operator!=(const AABB& other) const { return !(*this == other); }
	private:
		Point _center;
		Vector _radius;

		void validate() const;
	};

	inline AABB AABBLB(float left, float bottom, float width, float height) { return AABB(left + width / 2.0f, bottom + height / 2.0f, width, height); }
	inline AABB AABBCB(float centerX, float bottom, float width, float height) { return AABB(centerX, bottom + height / 2.0f, width, height); }
	inline AABB AABBLC(float left, float centerY, float width, float height) { return AABB(left + width / 2.0f, centerY, width, height); }

	/**********************************************************************************************
	 * Segment
	 *********************************************************************************************/
	class Segment : public Shape
	{
	public:
		static const Segment Zero;

		Segment(float x1, float y1, float x2, float y2) : _center((x1+x2)/2.0f, (y1+y2)/2.0f), _radius((x2-x1)/2.0f, (y2-y1)/2.0f) {}
		Segment(const Point& point1, const Point& point2) : _center((point1+point2)/2.0f), _radius((point2-point1)/2.0f) {}
		Segment(const Point& center, const Vector& radius);
		// No need for virtual destructor

		ShapeType::Enum getType() const { return ShapeType::SEGMENT; }

		const Point& getCenter() const { return _center; }
		const Vector& getRadius() const { return _radius; }
		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }
		float getRadiusVx() const { return getRadius().getVx(); }
		float getRadiusVy() const { return getRadius().getVy(); }

		Point getLeftPoint() const { return getCenter() + (getRadiusVx() <= 0.0f ? getRadius() : -getRadius()); }
		Point getRightPoint() const { return getCenter() + (getRadiusVx() > 0.0f ? getRadius() : -getRadius()); }
		Point getBottomPoint() const { return getCenter() + (getRadiusVy() <= 0.0f ? getRadius() : -getRadius()); }
		Point getTopPoint() const { return getCenter() + (getRadiusVy() > 0.0f ? getRadius() : -getRadius()); }
		Point getNaturalOrigin() const { return getRadiusVx() == 0.0f ? getBottomPoint() : getLeftPoint(); }
		Point getNaturalTarget() const { return getRadiusVx() == 0.0f ? getTopPoint() : getRightPoint(); }
		Vector getNaturalVector() const { return getNaturalTarget() - getNaturalOrigin(); }
		
		float getLeft() const { return getLeftPoint().getX(); }
		float getRight() const { return getRightPoint().getX(); }
		float getBottom() const { return getBottomPoint().getY(); }
		float getTop() const { return getTopPoint().getY(); }
		float getLength() const;
		float getY(float x) const { return get(Axis::Y, x); }
		float getX(float y) const  { return get(Axis::X, y); }

	private:
		Point _center;
		Vector _radius;

		float get(Axis::Enum axis, float otherAxisValue) const;
	};

	inline Segment SegmentPP(const Point& p1, const Point& p2) { return Segment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }

	/**********************************************************************************************
	 * Directed segment
	 *********************************************************************************************/
	class DirectedSegment
	{
	public:
		DirectedSegment(float x1, float y1, float x2, float y2) : _origin(x1, y1), _vector(x2 - x1, y2 - y1) {}
		DirectedSegment(const Point& origin, const Vector& vector) : _origin(origin), _vector(vector) {}
		const Point& getOrigin() const { return _origin; }
		const Vector& getVector() const { return _vector; }
		Point getTarget() const { return getOrigin() + getVector(); }
	private:
		Point _origin;
		Vector _vector;
	};

	inline DirectedSegment DirectedSegmentPP(const Point& p1, const Point& p2) { return DirectedSegment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }

	/**********************************************************************************************
	 * YABP - A parallelogram with 1 axis parallel to Y
	 *********************************************************************************************/
	class YABP : public Shape
	{
	public:
		YABP(const Point& center, const Vector& slopedRadius, float yRadius);

		const Point& getCenter() const { return _center; }
		const Vector& getSlopedRadius() const { return _slopedRadius; }
		float getYRadius() const { return _yRadius; }

		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		Vector getYVector() const { return Vector(0.0f, getYRadius()); }
		
		float getSlopedRadiusVx() const { return getSlopedRadius().getVx(); }
		float getSlopedRadiusVy() const { return getSlopedRadius().getVy(); }

		ShapeType::Enum getType() const { return ShapeType::YABP; }

		float getLeft() const {	return getCenterX() - getSlopedRadiusVx(); }
		float getRight() const { return getCenterX() + getSlopedRadiusVx(); }
		float getTop() const;
		float getBottom() const;

	private:
		Point _center;
		Vector _slopedRadius;
		float _yRadius;
		
		void validate() const;
	};
}
#endif