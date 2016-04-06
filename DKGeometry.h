#pragma once
#include <stdint.h>
#include <memory>
#include <vector>
#include <functional>
#include <limits>
#include <algorithm>
#include <iterator>

#define PI_F			3.14159265359f
#define PI_2_F			1.57079632679f
#define TORADIANS_F		1.74532925199e-002f

namespace DKGeometry
{
	class DRect;
	class DPoint;
#define DKInfinity std::numeric_limits<float>::infinity()
#define DKNegInfinity -std::numeric_limits<float>::infinity()
	struct DRange
	{
#define DRANGE_INVALID MAXINT32
		uint32_t start = 0;
		uint32_t length = 0;

		DRange() {}
		DRange(uint32_t sval, uint32_t lval) : start(sval), length(lval) {}
		DRange(uint32_t sizeValue) : start(0), length(sizeValue) {}

		DRange(const DWRITE_TEXT_RANGE&range) : start(range.startPosition), length(range.length) {}
		inline operator DWRITE_TEXT_RANGE() { return{ start, length }; }

		inline uint32_t end() const { return start + length - 1; } 
		inline bool isInvalid() const { return (start == DRANGE_INVALID); }

		inline DRange intersect(const DRange&range) const {
			uint32_t newstart = max(start, range.start);
			uint32_t newend = min(end(), range.end());
			if (newstart <= newend)
				return DRange(newstart, 1 + newend - newstart);
			return DRange(DRANGE_INVALID, 0);
		}
		
	};

	inline DRange ZeroRange() { return { 0, 0 }; }

	typedef std::vector<DRect>	DRectArray;

	typedef std::function<DKGeometry::DRectArray()> GetRectsFunc;

	DRect GetCombinedRect(const DRectArray&rectarray);

	inline bool closeToZero(float compare) 
	{
		return (fabsf(compare) < FLT_EPSILON);
	}

	inline int fCompare(float f1, float f2, float epsilon = FLT_EPSILON)
	{
		float diff = f1 - f2;
		if (fabsf(diff) < epsilon) return 0;
		return (diff < 0) ? -1 : 1;
	}

	inline float fixDim(float f)
	{
		float intpart, fractpart;
		fractpart = modff(f, &intpart);
		return (fractpart > 0.15f) ? intpart + 0.5f : intpart - 0.5f;
	}

	enum sideStatus
	{
		none			= 0,
		crossover		= 1,
		sharedSame		= 2,
		sharedOpposite	= 3
	};

	typedef union _RectComparision {
		struct {
			unsigned int interference : 1;
			unsigned int left : 1;
			unsigned int right : 1;
			unsigned int top : 1;
			unsigned int bottom : 1;
			unsigned int leftLeft : 1;
			unsigned int leftRight : 1;
			unsigned int rightLeft : 1;
			unsigned int rightRight : 1;
			unsigned int topTop : 1;
			unsigned int topBottom : 1;
			unsigned int bottomTop : 1;
			unsigned int bottomBottom : 1;
			unsigned int crossover : 1;
		};
		int32_t flat;
	} RectComparision;

	//DEFINE_ENUM_FLAG_OPERATORS(Edges)


	class DSize
	{
	public:
		float width;
		float height;

		/// <summary>
		/// Constructs a DSize object from two FLOAT values.</summary>
		/// <param name="fX">source X</param>
		/// <param name="fY">source Y</param>
		inline DSize(float fX = 0.f, float fY = 0.f) 
			: width(fX), height(fY) {}

		DSize(const DPoint&point);

		~DSize() {}

#ifdef _MFC_VER
		/// <summary>
		/// Constructs a DSize object from SIZE object.</summary>
		inline DSize(const SIZE& size) 
			: width((float)size.cx), height((float)size.cy) {}

		/// <summary>
		/// Constructs a DSize object from CD2DSizeF object.</summary>
		/// <param name="pt">source point</param>
		inline DSize(const CD2DSizeF& size) 
			: width(size.width) , height(size.height){}

		/// <summary>
		/// Converts DSize to CD2DSizeF object.</summary>
		/// <returns>
		/// Current value of D2D point.
		/// </returns>
		inline operator CD2DSizeF() { return CD2DSizeF(width, height); }
		inline operator SIZE() { SIZE result; result.cx = (LONG)width; result.cy = (LONG)height; return result; }
#endif // _MFC_VER

		inline bool operator==(const DSize&size) const {
			if (width != size.width) return false;
			return (height == size.height);
		}

		inline bool operator!=(const DSize&size) const {
			if (width == size.width) 
				if (height == size.height)
					return false;
			return true;
		}

		inline bool operator<(const float&sizef) const {
			return (width < sizef) || (height < sizef);
		}

		inline bool operator>(const float&sizef) const {
			return (width > sizef) || (height > sizef);
		}

	
		inline DSize operator*(const DSize&size) const { return DSize(width * size.width, height * size.height); }

		inline DSize operator+(const DSize&size) const { return DSize(width + size.width, height + size.height); }

		inline DSize operator-(const DSize&size) const { return DSize(width - size.width, height - size.height); }

		inline DSize operator*(const float&fAmount) const { return DSize(width * fAmount, height * fAmount); }

		inline DSize operator/(const float&fAmount) const { return DSize(width / fAmount, height / fAmount); }
		
		inline DSize operator*=(const float&fAmount) {
			width *= fAmount;
			height *= fAmount;
			return *this;
		}
		
		inline DSize operator+=(const DSize&size) {
			width += size.width;
			height += size.height;
			return *this;
		}
		
		inline DSize operator*=(const DSize&size) {
			width *= size.width;
			height *= size.height;
			return *this;
		}

		inline DSize operator-=(const DSize&size) {
			width -= size.width;
			height -= size.height;
			return *this;
		}

		inline bool isZeroSize()  const {
			return (width == 0) && (height == 0);
		}

		//operator DPoint();
		operator DPoint();

	private:

	};

	class DPoint
	{
	public:
		float x;
		float y;

		~DPoint() {}

		/// <summary>
		/// Constructs a DPoint object from DSize object.</summary>
		/// <param name="pt">source point</param>
		DPoint(const DSize& pt);

#ifdef _MFC_VER
		/// <summary>
		/// Constructs a DPoint object from CPoint object.</summary>
		/// <param name="pt">source point</param>
		inline DPoint(const CPoint& pt) 
			: x((float)pt.x), y((float)pt.y) {}

		/// <summary>
		/// Constructs a DPoint object from CD2DPointF object.</summary>
		/// <param name="pt">source point</param>
		inline DPoint(const CD2DPointF& pt) 
			: x((float)pt.x), y((float)pt.y) {}

		/// <summary>
		/// Constructs a DPoint object from CD2DSizeF object.</summary>
		/// <param name="pt">source point</param>
		inline DPoint(const CD2DSizeF& pt) 
			: x((float)pt.width), y((float)pt.height) {}

		/// <summary>
		/// Constructs a DPoint object from D2D1_POINT_2F object.</summary>
		/// <param name="pt">source point</param>
		inline DPoint(const D2D1_POINT_2F& pt) 
			: x((float)pt.x), y((float)pt.y) {}

		/// <summary>
		/// Constructs a DPoint object from D2D1_POINT_2F object.</summary>
		/// <param name="pt">source point</param>
		inline DPoint(const D2D1_POINT_2F* pt)
			: x((float)pt->x), y((float)pt->y) {}

		/// <summary>
		/// Converts DPoint to CPoint object.</summary>
		/// <returns>
		/// Current value of D2D point.
		/// </returns>
		inline operator CPoint()  const 
		{ return CPoint((LONG)x, (LONG)y); }

		/// <summary>
		/// Converts DPoint to CD2DPointF object.</summary>
		/// <returns>
		/// Current value of D2D point.
		/// </returns>
		inline operator CD2DPointF()  const { return CD2DPointF(x, y); }

		/// <summary>
		/// Converts DPoint to CD2DSizeF object.</summary>
		/// <returns>
		/// Current value of D2D point.
		/// </returns>
		inline operator CD2DSizeF()  const { return CD2DSizeF(x, y); }

		/// <summary>
		/// Converts DPoint to D2D1_POINT_2F</summary>
		/// <returns>
		/// Current value of DPoint.
		/// </returns>
		inline operator D2D1_POINT_2F()  const {
			D2D1_POINT_2F point;
			point.x = x;
			point.y = y;
			return point;
		}

#endif // _MFC_VER

		/// <summary>
		/// Constructs a DPoint object from two FLOAT values.</summary>
		/// <param name="fX">source X</param>
		/// <param name="fY">source Y</param>
		inline DPoint(float fX=0.f, float fY = 0.f) 
			: x(fX), y(fY) {}

		inline DPoint operator+(const DPoint& point) const {
			return DPoint(x + point.x, y + point.y);
		}

		inline DPoint operator-(const DPoint& point) const {
			return DPoint(x - point.x, y - point.y);
		}

		inline DPoint operator*(const DPoint& point) const {
			return DPoint(x * point.x, y * point.y);
		}

		inline DPoint operator/(const DPoint& point) const {
			return DPoint(x / point.x, y / point.y);
		}

		inline DPoint operator*(const float& fAmount) const {
			return DPoint(x * fAmount, y * fAmount);
		}

		inline DPoint operator/(const float& fAmount) const {
			return DPoint(x / fAmount, y / fAmount);
		}

		inline DPoint operator-() const {
			return DPoint(-x, -y);
		}

		void fix();

		inline DPoint fixed()  const { return DPoint(fixDim(x), fixDim(y)); }

		inline DPoint rotated(float radians, const DPoint&origin=DPoint(0,0)) const {
			float cA = cosf(radians);
			cA = closeToZero(cA) ? 0 : cA;
			float sA = sinf(radians);
			sA = closeToZero(sA) ? 0 : sA;

			float shiftx = x - origin.x;
			float shifty = y - origin.y;

			return DPoint(
				cA * shiftx - sA * shifty + origin.x,
				sA * shiftx + cA * shifty + origin.y
				);
		}

		inline DPoint getRotatedPoint(float angle, const DPoint&origin = DPoint(0, 0)) const
		{
			return rotated(angle * TORADIANS_F, origin);
		}
		

		void boundInRect(DRect rect);


	private:

	};

	inline DPoint operator*(const float& lhs, const DPoint& rhs) 
	{
		return DPoint(lhs * rhs.x, lhs * rhs.y);
	}


	class MLine {

		float s1;
		float s2;
		float m;
		float b;
		
		inline DPoint startPoint() {
			if ((m > 1) || (m < -1))
			{
				return { s1, (s1 * m) + b};
			}
			else {
				return { (s1 - b) / m, s1 };
			}
		}

		inline DPoint endPoint() {
			if ((m > 1) || (m < -1))
			{
				return{ s2, (s2 * m) + b };
			}
			else {
				return{ (s2 - b) / m, s2 };
			}
		}

		inline MLine(const DPoint&start, const DPoint&end) {
			float run = end.x - start.x;
			if (run) {
				float rise = end.y - start.y;
				m = rise / run;
				b = start.y - (start.x * m);

				if ((m > 1) || (m < -1))
				{
					s1 = start.x;
					s2 = end.x;
				}
				else {
					s1 = start.y;
					s2 = end.y;
				}
			}
			else {
				m = DKInfinity;
				b = end.x;
				s1 = start.x;
				s2 = end.x;
			}
		}

		inline MLine(float x1, float y1, float x2, float y2)
			: MLine(DPoint(x1,y1), DPoint(x2,y2)) {}

		inline MLine(float m, float b)
			: m(m), b(b), s1(DKNegInfinity), s2(DKInfinity) {}

		inline MLine(const DPoint&start, float m, float b, bool up)
			: m(m), b(b) {
			if ((m > 1) || (m < -1))
			{
				s1= start.y;
				if (up) s2 = DKInfinity;
				else s2 = DKNegInfinity;
			}
			else {
				s1 = start.x;
				if (up) s2 = DKInfinity;
				else s2 = DKNegInfinity;
			}
		}


	};

	class DLine
	{
	private:
		float m;
		float b;
	public:
		DPoint start;
		DPoint end;
		inline DLine() : start(DPoint(0, 0)), end(DPoint(0, 0)) {}
		inline DLine(const DPoint&start, const DPoint&end) : start(start), end(end) {}
		inline DLine(float x, float y, float i, float j) : start(DPoint(x,y)), end(DPoint(i,j)) {}

		inline bool isVertical() const {
			return (fCompare(start.x, end.x) == 0);
		}

		inline bool isHorizontal() const {
			return (fCompare(start.y, end.y) == 0);
		}


		static inline DLine verticalLine(float x) {
			DLine line(x, DKNegInfinity, x, DKInfinity);
			line.m = DKInfinity;
			line.b = x;
			return line;
		}

		static inline DLine horizontalLine(float y){
			DLine line(DKNegInfinity, y, DKInfinity, y);
			line.m = 0;
			line.b = y;
			return line;
		}

		static inline DLine slopedLine(float m, float b) {	
			if (m > 0)
			{
				DLine line(DKNegInfinity,DKNegInfinity, DKInfinity, DKInfinity);
				line.m = m;
				line.b = b;
				return line;
			}
			else if (m == 0)
			{
				DLine line(DKNegInfinity, b, DKInfinity, b);
				line.m = m;
				line.b = b;
				return line;
			}
			else {
				DLine line(DKNegInfinity, DKInfinity, DKInfinity, DKNegInfinity);
				line.m = m;
				line.b = b;
				return line;
			}
			return DLine();
		}

		inline float slope(float&b) const {
			if (fCompare(start.x, end.x) == 0)
			{
				b = start.x;
				return DKInfinity;
			}
			float m = (end.y - start.y) / (end.x - start.x);
			b = start.y - m * start.x;
			return m;
		}

		inline bool xInLine(float x) const
		{ 
			return (x >= start.x && x <= end.x) || 
				(x >= end.x && x <= start.x);
		}

		inline bool yInLine(float y) const
		{
			return (y >= start.y && y <= end.y) ||
				(y >= end.y && y <= start.y);
		}

		inline bool containsPoint(const DPoint&point) const
		{
			float b1;
			float m1 = slope(b1);

			float y = m1 * point.x + b1;

			if (fCompare(y, point.y) == 0)
			{
				return yInLine(y);
			}

			return false;
		}


		inline bool crosses(const DLine&line) const {

			float b1;
			float m1 = slope(b1);

			float b2;
			float m2 = line.slope(b2);

			float x, y;

			if (std::isinf(m1))
			{
				if (std::isinf(m2))
				{
					return false; // parallel vertical lines
				}

				x = b1;
				y = x * m2 + b2;
			} else {

				if (std::isinf(m2))
				{
					x = b2;
					y = x * m1 + b1;
				}
				else {
					if (fCompare(m1, m2) == 0) return false; // parallel lines
					x = (b1 - b2) / (m2 - m1);
					y = m1 * x + b1;
				}
			}

			return (xInLine(x) && line.xInLine(x));
		}

	};

	class DRect
	{
	public:
		float	left;
		float	top;
		float	right;
		float	bottom;

		~DRect() {}

		inline DRect(const DPoint& origin, const DSize& size) 
			: left(origin.x), top(origin.y),
			right(origin.x + size.width),
			bottom(origin.y + size.height) {}
		
		inline DRect(const DPoint& topLeft, const DPoint& bottomRight)
			: left(topLeft.x), top(topLeft.y),
			right(bottomRight.x),
			bottom(bottomRight.y) {}

		inline DRect(const DSize& size)
			:left(0), top(0), 
			right(size.width), bottom(size.height) {}



#ifdef _MFC_VER

		/// <summary>
		/// Constructs a DRect object from CRect object.</summary>
		/// <param name="rect">source rectangle</param>
		inline DRect(const CRect& rect) 
			: left((float)rect.left),
			top((float)rect.top),
			right((float)rect.right),
			bottom((float)rect.bottom) {}

		/// <summary>
		/// Constructs a DRect object from CD2DRectF object.</summary>
		/// <param name="rect">source rectangle</param>
		inline DRect(const CD2DRectF& rect)
			: left((float)rect.left),
			top((float)rect.top),
			right((float)rect.right),
			bottom((float)rect.bottom) {}

		/// <summary>
		/// Constructs a DRect object from D2D1_RECT_F object.</summary>
		/// <param name="rect">source rectangle</param>
		inline DRect(const D2D1_RECT_F& rect)
			: left((float)rect.left),
			top((float)rect.top),
			right((float)rect.right),
			bottom((float)rect.bottom) {}

		/// <summary>
		/// Constructs a DRect object from D2D1_RECT_F object.</summary>
		/// <param name="rect">source rectangle</param>
		inline DRect(const D2D1_RECT_F* rect)
			: left((float)rect->left),
			top((float)rect->top),
			right((float)rect->right),
			bottom((float)rect->bottom) {}

		/// <summary>
		/// Converts CD2DRectF to CRect object.</summary>
		/// <returns>
		/// Current value of D2D rectangle.
		/// </returns>
		operator CRect()  const { return CRect((LONG)left, (LONG)top, (LONG)right, (LONG)bottom); }

		/// <summary>
		/// Converts CD2DRectF to CD2DRectF object.</summary>
		/// <returns>
		/// Current value of D2D rectangle.
		/// </returns>
		operator CD2DRectF()  const { return CD2DRectF(left, top, right, bottom); }

		/// <summary>
		/// Converts CD2DRectF to D2D1_RECT_F</summary>
		/// <returns>
		/// Current value of D2D rectangle.
		/// </returns>
		operator D2D1_RECT_F()  const {
			D2D1_RECT_F rect;
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;
			return rect;
		}

#endif // _MFC_VER

		/// <summary>
		/// Constructs a DRect object from four FLOAT values.</summary>
		/// <param name="fLeft">source left coordinate</param>
		/// <param name="fTop">source top coordinate</param>
		/// <param name="fRight">source right coordinate</param>
		/// <param name="fBottom">source bottom coordinate</param>
		inline DRect(float fLeft = 0., float fTop = 0., float fRight = 0., float fBottom = 0.) 
			: left(fLeft), top(fTop), right(fRight), bottom(fBottom) {
		}

		/// <summary>
		/// Returns a Boolean value that indicates whether an expression contains no valid data (Null).</summary>
		/// <returns>
		/// TRUE if rectangle's top, left, bottom, and right values are all equal to 0; otherwise FALSE.
		/// </returns>
		bool IsNull() const { return left == 0. && right == 0. && top == 0. && bottom == 0.; }



		// Union of Two DRects
		DRect operator+(const DRect& rect)  const {
			DRect temp;
			temp.left = (left < rect.left) ? left : rect.left;
			temp.top = (top < rect.top) ? top : rect.top;
			temp.right = (right > rect.right) ? right : rect.right;
			temp.bottom = (bottom > rect.bottom) ? bottom : rect.bottom;

			return temp;
		}

		inline bool operator==(const DRect& rect)  const {
			return (left == rect.left && right == rect.right && top == rect.top && bottom == rect.bottom);
		}

		// Move Rect by point magnitude
		inline DRect operator+(const DPoint& point)  const {
			return DRect(left + point.x, top + point.y, right + point.x, bottom + point.y);
		}

		inline DRect operator+(const DSize& size)  const {
			return DRect(left + size.width, top + size.height, right + size.width, bottom + size.height);
		}

		inline DRect operator+=(const DPoint& point)  {
			left += point.x;
			right += point.x;
			top += point.y;
			bottom += point.y;
			return *this;
		}

		inline DRect operator+=(const DSize& size) {
			left += size.width;
			right += size.width;
			top += size.height;
			bottom += size.height;
			return *this;
		}

		inline DRect operator*=(const DPoint& point) {
			left *= point.x;
			right *= point.x;
			top *= point.y;
			bottom *= point.y;
			return *this;
		}


		// Scale Rect By Point Magnitude
		DRect operator*(const DPoint& point) const {
			return DRect(left * point.x, top * point.y, right * point.x, bottom * point.y);
		}

		// Scale Rect By Value
		DRect operator*(const float& scale) const {
			return DRect(left * scale, top * scale, right * scale, bottom * scale);
		}
		
		inline float Width() const { return right - left;  }
		inline float Height() const { return bottom - top; }
		inline float area() const { return (right - left) * (bottom - top); }
		inline DPoint origin() const { return DPoint(left, top); }
		inline DPoint topLeft() const { return DPoint(left, top); }
		inline DPoint topRight() const { return DPoint(right, top); }
		inline DPoint bottomLeft() const { return DPoint(left, bottom); }
		inline DPoint bottomRight() const  { return DPoint(right, bottom); }
		inline DPoint center() const { return DPoint((left + right) / 2.f, (top + bottom) / 2.f); }


		inline DSize size() const { return DSize(right - left, bottom - top); }
		inline void setWidth(float newWidth) { right = left + newWidth; }
		inline void setHeight(float newHeight) { bottom = top + newHeight; }

		inline void setSize(const DKGeometry::DSize&size) { right = left + size.width; bottom = top + size.height; }

		inline void setBottomRight(const DPoint& bottomRight) {
			bottom = bottomRight.y;
			right = bottomRight.x;
		}


		inline void setTopLeft(const DPoint& topLeft) {
			top = topLeft.y;
			left = topLeft.x;
		}

		inline void fix() {
			left = fixDim(left);
			right = fixDim(right);
			top = fixDim(top);
			bottom = fixDim(bottom);
		}

		inline float diagonalLength() { return sqrtf(Width() * Width() + Height() * Height()); }

		inline DRect fixed()  const { 
			return DRect(topLeft().fixed(), bottomRight().fixed());
		}

		inline std::vector<DPoint> getPoints() const {
			return { topLeft(), topRight(), bottomLeft(), bottomRight() };
		}

		inline std::vector<DPoint> getRotatedPoints(float radians, const DPoint&origin=DPoint(0,0)) const {
			auto points = getPoints();
			decltype(points) result;
			auto pOrigin = &origin;
			std::transform(points.begin(), points.end(), std::back_inserter(result),
			[radians, pOrigin](auto&eachPoint) { 
				return eachPoint.rotated(radians, *pOrigin);
			});

			return result;
		}


		inline DRect getRotatedBounds(float angle, DPoint origin=DPoint(0, 0)) const
		{
			// there is a more efficient way to do this, but this is a simple method that works
			DRect bounds(DKInfinity, DKInfinity, DKNegInfinity, DKNegInfinity);
			for (auto&eachPoint : getPoints())
			{
				auto rotatedPoint = eachPoint.getRotatedPoint(angle, origin);
				if (rotatedPoint.x < bounds.left) bounds.left = rotatedPoint.x;
				if (rotatedPoint.x > bounds.right) bounds.right = rotatedPoint.x;
				if (rotatedPoint.y < bounds.top) bounds.top = rotatedPoint.y;
				if (rotatedPoint.y > bounds.bottom) bounds.bottom = rotatedPoint.y;
			}
			bounds.Normalize();
			return bounds;
		}

		inline DSize getRotateShift(float angle, DPoint origin = DPoint(0, 0)) const
		{
			auto baseRect = getRotatedBounds(angle, origin);
			baseRect.MoveOrigin(0,0);
			return -baseRect.getRotatedBounds(angle).topLeft();
		}

		inline void Move(float xAmount, float yAmount) {
			left += xAmount;
			right += xAmount;
			top += yAmount;
			bottom += yAmount;
		}

		//inline void Move(float )

		inline void Grow(float xAmount, float yAmount) {
			left -= xAmount / 2;
			top -= xAmount / 2;
			right += xAmount / 2;
			bottom += yAmount / 2;
		}

		inline void addToBottomRight(DSize&size) { right += size.width; bottom += size.height; }

		void MoveOrigin(float newX, float newY);
		void MoveBottomRight(float newRight, float newBottom);

		void MoveCenter(float newX, float newY);

		void MoveXOrigin(float newX);
		void MoveYOrigin(float newY);


		DRect Combine(const DRect&rect);
		void CombineWith(const DRect&rect);

		bool Intersection(DRect rect, DRect &intersectRect, bool ignoreLine = true) const;
		bool Intersects(DRect rect) const;
		bool IsContainedIn(DRect rect) const;
		bool test(float l, float t, float r, float b, int testType = 0);
		void Normalize();
		inline bool isNormal() const {
			return (right >= left) && (bottom >= top);
		}

		inline bool PointInRect(float x, float y) const {
			return (x >= left && x <= right && y >= top && y <= bottom);

		}

		std::string toString() const;

		void shrink(float toPercent);


		inline bool PointInRect(const DPoint&point) const { 
			return (point.x >= left) && (point.x <= right) && (point.y >= top) && (point.y <= bottom); 
		}


		inline DLine topLine() const {
			return DLine(topLeft(), topRight());
		}

		inline DLine leftLine() const {
			return DLine(topLeft(), bottomLeft());
		}

		inline DLine bottomLine() const {
			return DLine(bottomLeft(), bottomRight());
		}

		inline DLine rightLine() const {
			return DLine(topRight(), bottomRight());
		}

		inline std::vector<DLine> getLines() const {
			return { topLine(), leftLine(), bottomLine(), rightLine() };
		}

		inline bool LineCrossesRect(const DLine&line) const {
			return (line.crosses(topLine()) || line.crosses(leftLine()) ||
				line.crosses(bottomLine()) || line.crosses(rightLine()));
		}

		bool IsErrorRect() const;
	
		DKGeometry::RectComparision compareToRect(DRect rect, bool getSharedEdges = false) const;

	private:

	};


	class IDRect : public DRect
	{
	public:
		uint64_t id;
		inline IDRect(const DRect&rect, uint64_t id=0) : DRect(rect), id(id) {}
		inline IDRect() : DRect(), id(0) {}
		inline void setID(uint64_t id) { this->id = id; }
		inline uint64_t getID() const { return id; }
		inline void setRect(const DRect&rect) {
			left = rect.left;
			top = rect.top;
			right = rect.right;
			bottom = rect.bottom;
		}
	};

	typedef std::vector<IDRect> IDRArray;

	DRect ERROR_RECT();
	DRect INFINITY_RECT();

	bool test();

	inline DKGeometry::DSize::DSize(const DPoint & point) 
		: width(point.x), height(point.y) {}

	//inline operator DKGeometry::DSize::DPoint() { return DPoint(width, height); }

	inline DRect CreateCenteredRect(DKGeometry::DPoint&center, DKGeometry::DSize size) {
		DKGeometry::DPoint origin(center.x - size.width / 2, center.y - size.height / 2);
		return DRect(origin, size);
	}

	inline DRect CreateCenteredRect(const DKGeometry::DPoint&center, float baseSize) {
		DKGeometry::DSize size(baseSize, baseSize);
		DKGeometry::DPoint origin(center.x - size.width / 2, center.y - size.height / 2);
		return DRect(origin, size);
	}

}




