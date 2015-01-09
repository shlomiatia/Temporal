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
		Segment(float x1, float y1, float x2, float y2);
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
		Vector getNaturalDirection() const { return getNaturalVector().normalize(); }

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }
		
		float getLeft() const { return getLeftPoint().getX(); }
		float getRight() const { return getRightPoint().getX(); }
		float getBottom() const { return getBottomPoint().getY(); }
		float getTop() const { return getTopPoint().getY(); }
		float getLength() const { return getRadius().getLength() * 2.0f; }

		float getY(float x) const { return get(Axis::Y, x); }
		float getX(float y) const  { return get(Axis::X, y); }

		float getWidth() const { return getRadiusX() * 2.0f; }
		float getHeight() const { return getRadiusY() * 2.0f; }

		Segment* clone() const { return new Segment(_center, _radius); }

		void translate(const Vector& translation) { _center += translation; }
		void rotate(Side::Enum orientation) { _center.setX(_center.getX() * orientation); }

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
		void setCenterX(float x) { _center.setX(x); }
		void setCenterY(float y) { _center.setY(y); }

		const Vector& getRadius() const { return _radius; }
		float getRadiusX() const { return getRadius().getX(); }
		float getRadiusY() const { return getRadius().getY(); }
		void setRadiusX(float x) { _radius.setX(x); }
		void setRadiusY(float y) { _radius.setY(y); }

		float getBottom() const { return getCenterY() - getRadiusY(); }
		float getLeft() const {	return getCenterX() - getRadiusX(); }
		float getTop() const { return getCenterY() + getRadiusY(); }
		float getRight() const { return getCenterX() + getRadiusX(); }
		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return getSide(Side::getOpposite(orientation)); }
		float getWidth() const { return getRadiusX() * 2.0f; }
		float getHeight() const { return getRadiusY() * 2.0f; }
		void setWidth(float width) { setRadiusX(width / 2.0f); }
		void setHeight(float height) { setRadiusY(height / 2.0f); }

		AABB* clone() const { return new AABB(_center, _radius); }

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
	 * OBB 
	 *********************************************************************************************/
	class OBBAABBWrapper;

	class OBB 
	{
	public:
		static const OBB Zero;

		OBB () {}

		OBB(const Vector& center, float angle, const Vector& radius) : _center(center), _radius(radius) 
		{
			setAngle(angle);
		}

		OBB(const Vector& center, const Vector& axis0, const Vector& radius) : _center(center), _radius(radius) 
		{
			setAxis0(axis0);
		}

		const Vector& getCenter() const { return _center; }
		void setCenter(const Vector& center) { _center = center; }
		float getCenterX() const { return getCenter().getX(); }
		float getCenterY() const { return getCenter().getY(); }
		void setCenterX(float x) { _center.setX(x); }
		void setCenterY(float y) { _center.setY(y); }

		const Vector& getRadius() const { return _radius; }
		float getRadiusX() const { return getRadius().getX(); }
		float getRadiusY() const { return getRadius().getY(); }
		void setRadius(const Vector& radius) { _radius = radius; }
		void setRadiusX(float x) { _radius.setX(x); }
		void setRadiusY(float y) { _radius.setY(y); }

		const Vector* getAxes() const { return _axes; }
		const Vector& getAxis(Axis::Enum i) const { return _axes[i]; }
		const Vector& getAxisX() const { return getAxis(Axis::X); }
		const Vector& getAxisY() const { return getAxis(Axis::Y); }

		void translate(const Vector& translation) { _center += translation; }

		Vector getBottomLeftVertex() const { return getCenter() - getAxisX() * getRadiusX() - getAxisY() * getRadiusY(); }
		Vector getTopLeftVertex() const { return getCenter() - getAxisX() * getRadiusX() + getAxisY() * getRadiusY(); } 
		Vector getBottomRightVertex() const { return getCenter() + getAxisX() * getRadiusX() - getAxisY() * getRadiusY(); }
		Vector getTopRightVertex() const { return getCenter() + getAxisX() * getRadiusX() + getAxisY() * getRadiusY(); }
		
		float getLeft() const { return getTopLeftVertex().getX(); }
		float getRight() const { return getBottomRightVertex().getX(); }
		float getBottom() const { return getBottomLeftVertex().getY(); }
		float getTop() const { return getTopRightVertex().getY(); }
		float getSide(Side::Enum side) const { return side == Side::LEFT ? getLeft() : getRight(); }
		OBBAABBWrapper getAABBWrapper();
		const OBBAABBWrapper getAABBWrapper() const;

		float getAngle() const { return _axes[0].getAngle(); }

		// angle>=0<90
		void setAngle(float angle)
		{
			setAxis0(Vector(angle));
		}

		// axis0 => unit vector. x>0, y>=0
		void setAxis0(const Vector& axis0);
	private:
		Vector _center;
		Vector _axes[2];
		Vector _radius;

		friend class SerializationAccess;

	};

	class OBBAABBWrapper
	{
	public:
		// TODO: Validate
		explicit OBBAABBWrapper(OBB* obb = 0) : _obb(obb) {}

		const Vector& getCenter() const { return _obb->getCenter(); }
		float getCenterX() const { return _obb->getCenter().getX(); }
		float getCenterY() const { return _obb->getCenter().getY(); }

		const Vector& getRadius() const { return _obb->getRadius(); }
		float getRadiusX() const { return _obb->getRadius().getX(); }
		float getRadiusY() const { return _obb->getRadius().getY(); }

		float getBottom() const { return _obb->getCenterY() - _obb->getRadiusY(); }
		float getLeft() const {	return _obb->getCenterX() - _obb->getRadiusX(); }
		float getTop() const { return _obb->getCenterY() + _obb->getRadiusY(); }
		float getRight() const { return _obb->getCenterX() + _obb->getRadiusX(); }
		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return getSide(Side::getOpposite(orientation)); }
		float getWidth() const { return _obb->getRadiusX() * 2.0f; }
		float getHeight() const { return _obb->getRadiusY() * 2.0f; }
		void setWidth(float width) { _obb->setRadiusX(width / 2.0f); }
		void setHeight(float height) { _obb->setRadiusY(height / 2.0f); }

		void setOBB(OBB& obb) { _obb = &obb; }
		OBB& getOBB() const { return *_obb; }

	private:
		OBB* _obb;
	};

	inline OBB OBBAABB(const Vector& center, const Vector& radius) { return OBB(center, 0.0f, radius); }

	/**********************************************************************************************
	 * Directed segment
	 *********************************************************************************************/
	class DirectedSegment
	{
	public:
		DirectedSegment(float x1, float y1, float x2, float y2) : _origin(x1, y1), _vector(x2 - x1, y2 - y1) {}
		DirectedSegment(const Vector& origin, const Vector& vector) : _origin(origin), _vector(vector) {}
		const Vector& getOrigin() const { return _origin; }
		void setOrigin(const Vector& origin) { _origin = origin; }
		const Vector& getVector() const { return _vector; }
		void setVector(const Vector& vector) { _vector = vector; }
		Vector getTarget() const { return getOrigin() + getVector(); }
		Vector getDirection() const { return getVector().normalize(); }
	private:
		Vector _origin;
		Vector _vector;
	};

	inline DirectedSegment DirectedSegmentPP(const Vector& p1, const Vector& p2) { return DirectedSegment(p1.getX(), p1.getY(), p2.getX(), p2.getY()); }
}
#endif