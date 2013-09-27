#ifndef SHAPES_H
#define SHAPES_H

#include "BaseEnums.h"
#include "Vector.h"
#include <math.h>

namespace Temporal
{
	class SerializationAccess;

	
	/**********************************************************************************************
	 * Segment
	 *********************************************************************************************/
	class Segment
	{ 
	public:
		static const Segment Zero;
		 
		Segment() {}
		Segment(float x1, float y1, float x2, float y2) : _center((x1+x2)/2.0f, (y1+y2)/2.0f), _radius((x2-x1)/2.0f, (y2-y1)/2.0f) {}
		Segment(const Vector& center, const Vector& radius);
		
		const Vector& getCenter() const { return _center; }
		void setCenter(const Vector& center) { _center = center; }
		
		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		const Vector& getRadius() const { return _radius; }
		float getRadiusX() const { return getRadius().getX(); }
		float getRadiusY() const { return getRadius().getY(); }

		Vector getLeftPoint() const { return getCenter() + (getRadiusX() < 0.0f ? getRadius() : -getRadius()); }
		Vector getRightPoint() const { return getCenter() + (getRadiusX() >= 0.0f ? getRadius() : -getRadius()); }
		Vector getBottomPoint() const { return getCenter() + (getRadiusY() <= 0.0f ? getRadius() : -getRadius()); }
		Vector getTopPoint() const { return getCenter() + (getRadiusY() > 0.0f ? getRadius() : -getRadius()); }

		Vector getPoint(Side::Enum side) const { return side == Side::LEFT ? getLeftPoint() : getRightPoint(); }

		Vector getNaturalOrigin() const { return getRadiusX() == 0.0f ? getBottomPoint() : getLeftPoint(); }
		Vector getNaturalTarget() const { return getRadiusX() == 0.0f ? getTopPoint() : getRightPoint(); }
		Vector getNaturalVector() const { return getNaturalTarget() - getNaturalOrigin(); }

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }
		
		float getLeft() const { return getLeftPoint().getX(); }
		float getRight() const { return getRightPoint().getX(); }
		float getBottom() const { return getBottomPoint().getY(); }
		float getTop() const { return getTopPoint().getY(); }
		float getLength() const;

		float getY(float x) const { return get(Axis::Y, x); }
		float getX(float y) const  { return get(Axis::X, y); }

		float getWidth() const { return getRadiusX() * 2.0f; }
		float getHeight() const { return getRadiusY() * 2.0f; }

		Segment* clone() const { return new Segment(_center, _radius); }

		void translate(const Vector& translation) { _center += translation; }
		void rotate(Side::Enum orientation) { _center.setX(_center.getX() * orientation); }

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("center", _center);	
			serializer.serialize("radius", _radius);
		}

	private:
		Vector _center;
		Vector _radius;

		float get(Axis::Enum axis, float otherAxisValue) const;

		friend class SerializationAccess;
	};

	inline Segment SegmentPP(const Vector& p1, const Vector& p2) { return Segment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }

	/**********************************************************************************************
	 * AABB
	 *********************************************************************************************/
	class AABB
	{
	public:
		static const AABB Zero;

		AABB() {}
		AABB(float centerX, float centerY, float width, float height);
		AABB(const Vector& center, const Vector& radius);

		const Vector& getCenter() const { return _center; }
		void setCenter(const Vector& center) { _center = center; }

		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }

		const Vector& getRadius() const { return _radius; }

		float getRadiusVx() const { return getRadius().getX(); }
		float getRadiusVy() const { return getRadius().getY(); }

		float getBottom() const { return getCenterY() - getRadiusVy(); }
		float getLeft() const {	return getCenterX() - getRadiusVx(); }
		float getTop() const { return getCenterY() + getRadiusVy(); }
		float getRight() const { return getCenterX() + getRadiusVx(); }
		
		AABB* clone() const { return new AABB(_center, _radius); }

		float getWidth() const { return getRadiusVx() * 2.0f; }
		float getHeight() const { return getRadiusVy() * 2.0f; }

		bool operator==(const AABB& other) const { return ((getCenter() == other.getCenter()) && (getRadius() == other.getRadius())); }
		bool operator!=(const AABB& other) const { return !(*this == other); }

		void translate(const Vector& translation) { _center += translation; }
		void rotate(Side::Enum orientation) { _center.setX(_center.getX() * orientation); }
		
	private:
		Vector _center;
		Vector _radius;

		void validate() const;

		friend class SerializationAccess;
	};

	inline AABB AABBLT(float left, float top, float width, float height) { return AABB(left + width / 2.0f, top - height / 2.0f, width, height); }

	/**********************************************************************************************
	 * YABP - A parallelogram with 1 axis parallel to Y
	 *********************************************************************************************/
	class YABP
	{
	public:
		static const YABP Zero;

		YABP() {}
		YABP(const Vector& center, const Vector& slopedRadius, float yRadius);

		const Vector& getCenter() const { return _center; }
		void setCenter(const Vector& center) { _center = center; }
		const Vector& getSlopedRadius() const { return _slopedRadius; }
		void setSlopedRadius(const Vector& slopedRadius){ _slopedRadius = slopedRadius; }
		float getYRadius() const { return _yRadius; }
		void setYRadius(float yRadius) { _yRadius = yRadius; }

		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }

		float getSlopedRadiusVx() const { return getSlopedRadius().getX(); }
		float getSlopedRadiusVy() const { return getSlopedRadius().getY(); }

		Vector getYVector() const { return Vector(0.0f, getYRadius()); }

		float getLeft() const {	return getCenterX() - getSlopedRadiusVx(); }
		float getRight() const { return getCenterX() + getSlopedRadiusVx(); }
		float getTop() const;
		float getBottom() const;

		float getWidth() const { return getSlopedRadiusVx() * 2.0f; }
		float getHeight() const { return (abs(getSlopedRadiusVy()) + getYRadius()) * 2.0f; }

		Vector getTopLeft() const { return getCenter() - getSlopedRadius() + getYVector(); }
		Vector getBottomLeft() const { return getCenter() - getSlopedRadius() - getYVector(); }
		Vector getTopRight() const { return getCenter() + getSlopedRadius() + getYVector(); }
		Vector getBottomRight() const { return getCenter() + getSlopedRadius() - getYVector(); }

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }
		float getTop(Side::Enum side) const { return getCenterY() + (Side::LEFT ? -1.0f : 1.0f) * getSlopedRadiusVy() + getYRadius(); }
		float getBottom(Side::Enum side) const { return getCenterY() + (Side::LEFT ? -1.0f : 1.0f) * getSlopedRadiusVy() - getYRadius(); }
		Vector getTopSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getTopLeft() : getTopRight(); }
		Vector getBottomSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getBottomLeft() : getBottomRight(); }

		void translate(const Vector& translation) { _center += translation; }
		void rotate(Side::Enum orientation) { _center.setX(_center.getX() * orientation); }

		Segment getTopSegment() const { return Segment(getCenter() + getYVector(), getSlopedRadius()); }
		Segment getBottomSegment() const { return Segment(getCenter() - getYVector(), getSlopedRadius()); }

		YABP* clone() const { return new YABP(_center, _slopedRadius, _yRadius); }
	private:
		Vector _center;
		Vector _slopedRadius;
		float _yRadius;
		
		void validate() const;

		friend class SerializationAccess;
	};

	inline YABP YABPAABB(const Vector& center, const Vector& radius) { return YABP(center, Vector(radius.getX(), 0.0f), radius.getY()); }

	/**********************************************************************************************
	 * Directed segment
	 *********************************************************************************************/
	class DirectedSegment
	{
	public:
		DirectedSegment(float x1, float y1, float x2, float y2) : _origin(x1, y1), _vector(x2 - x1, y2 - y1) {}
		DirectedSegment(const Vector& origin, const Vector& vector) : _origin(origin), _vector(vector) {}
		const Vector& getOrigin() const { return _origin; }
		const Vector& getVector() const { return _vector; }
		Vector getTarget() const { return getOrigin() + getVector(); }
	private:
		Vector _origin;
		Vector _vector;
	};

	inline DirectedSegment DirectedSegmentPP(const Vector& p1, const Vector& p2) { return DirectedSegment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }
}
#endif