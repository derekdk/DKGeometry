/*
MIT License

Copyright (c) 2016 Derek Dean Kowaluk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifdef _MFC_VER
#include "stdafx.h"
#endif

#include "DKGeometry.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace DKGeometry;


void DKGeometry::DRect::MoveOrigin(float newX, float newY)
{
	right = newX + right - left;
	left = newX;

	bottom = newY + bottom - top;
	top = newY;
}

void DKGeometry::DRect::MoveBottomRight(float newRight, float newBottom)
{
	left = newRight - (right - left);
	right = newRight;

	top = newBottom - (bottom - top);
	bottom = newBottom;

}

void DKGeometry::DRect::MoveCenter(float newX, float newY)
{
	float diffX = newX - ((left + right) / 2);
	float diffY = newY - ((top + bottom) / 2);

	left += diffX;
	right += diffX;
	top += diffY;
	bottom += diffY;
}

void DKGeometry::DRect::MoveXOrigin(float newX)
{
	right = newX + right - left;
	left = newX;
}

void DKGeometry::DRect::MoveYOrigin(float newY)
{
	bottom = newY + bottom - top;
	top = newY;
}


DRect DKGeometry::DRect::Combine(const DRect&rect)
{
	return DRect(min(left, rect.left), min(top, rect.top), max(right, rect.right), max(bottom, rect.bottom));
}

void DKGeometry::DRect::CombineWith(const DRect & rect)
{
	left = min(left, rect.left);
	top = min(top, rect.top);
	right = max(right, rect.right);
	bottom = max(bottom, rect.bottom);
}

bool DKGeometry::DRect::Intersection(DRect rect, DRect & intersectRect, bool ignoreLine) const
{

	//** first normalize!!
	DRect temp(*this);
	DRect compRect(rect);
	temp.Normalize();
	compRect.Normalize();

	if (
		temp.right < compRect.left ||
		compRect.right < temp.left ||
		temp.bottom < compRect.top ||
		compRect.bottom < temp.top
		) {
		// No overlap
		return FALSE;
	}
	DRect resultRect(
		(temp.left > compRect.left) ? temp.left : compRect.left,
		(temp.top > compRect.top) ? temp.top : compRect.top,
		(compRect.right < temp.right) ? compRect.right : temp.right,
		(compRect.bottom < temp.bottom) ? compRect.bottom : temp.bottom
		);

	intersectRect = resultRect;

	if (ignoreLine)
	{
		if (closeToZero(resultRect.Width()) || closeToZero(resultRect.Height()))
		{
			return FALSE;
		}
	}

	return TRUE;
}

bool DKGeometry::DRect::Intersects(DRect rect) const
{

	DRect temp(*this);
	temp.Normalize();
	rect.Normalize();

	return !(
		temp.right < rect.left ||
		rect.right < temp.left ||
		temp.bottom < rect.top ||
		rect.bottom < temp.top
		);
}

bool DKGeometry::DRect::IsContainedIn(DRect rect) const
{
	DRect temp(*this);
	temp.Normalize();
	rect.Normalize();

	return (temp.left >= rect.left &&
		temp.right <= rect.right &&
		temp.top >= rect.top &&
		temp.bottom <= rect.bottom);
}

bool DKGeometry::DRect::test(float l, float t, float r, float b, int testType)
{
	DRect compareTo(l, t, r, b);

	switch (testType)
	{
	case 0:
		return compareToRect(compareTo).crossover == 1;
		break;
	default:
		break;
	}

	return false;
}




void DKGeometry::DRect::Normalize()
{
	if (left > right) {
		float temp = left;
		left = right;
		right = temp;
	}
	if (top > bottom) {
		float temp = top;
		top = bottom;
		bottom = temp;
	}
}

std::string DKGeometry::DRect::toString() const
{
	std::stringstream ss;
	ss << "Rect:(" << left << "," << top << ")(" << Width() << "," << Height() << ")";

	return ss.str();
}

void DKGeometry::DRect::shrink(float toPercent)
{
	float oldwidth = right - left;
	float oldheight = bottom - top;

	float newwidth = oldwidth * toPercent;
	float newheight = oldheight * toPercent;

	float xdiff = -(newwidth - oldwidth)/2;
	float ydiff = -(newheight - oldheight)/2;

	left += xdiff;
	top += ydiff;
	right = left + newwidth;
	bottom = top + newheight;
}

bool DKGeometry::DRect::IsErrorRect() const
{
	return (*this) == ERROR_RECT();
}

DKGeometry::RectComparision DKGeometry::DRect::compareToRect(DRect rect, bool getSharedEdges) const
{
	RectComparision result;

	result.flat = 0;

	
	int leftright = fCompare(left, rect.right);
	int rightleft = fCompare(right, rect.left);
	int topbottom = fCompare(top, rect.bottom);
	int bottomtop = fCompare(bottom, rect.top);
	int rightright = fCompare(right, rect.right);
	int bottombottom = fCompare(bottom, rect.bottom);

	result.interference = !(
		rightleft == -1 || leftright == 1 || bottomtop == -1 || topbottom == 1
		);

	if (result.interference == 0) return result;

	int leftleft		= fCompare(left,	rect.left);
	int toptop			= fCompare(top,		rect.top);
	
	result.crossover = (
		(leftleft == 1 && leftright == -1) || leftleft == -1 && rightleft == 1) &&
		(toptop == 1 && topbottom == -1 || toptop == -1 && bottomtop == 1);


	
	/**
		s1: Crosses Right Edge
		L1   L2   R1   R2
		| -- | -- | -- |
		left < rect.left < right < rect.right

		s1a: Crosses Right Edge
		L1
		L2   R1   R2
		| -- | -- |

		left == rect.left < right < rect.right

		s2: Inside
		L1   L2   R2   R1
		| -- | -- | -- |

		left < rect.left < rect.right < right

		s2a: Inside
		L1
		L2   R2   R1
		| -- | -- |

		left == rect.left < rect.right < right

		s2b: Inside
				  R1
		L1   L2   R2
		| -- | -- |
		left < rect.left < rect.right == right

		s2c: Inside
		L1   R1
		L2   R2
		| -- |
		left == rect.left < rect.right == right
		

		s3: Crosses Left Edge
		L2   L1   R2   R1
		| -- | -- | -- |

		rect.left < left < rect.right < right

		s3a: Crosses Left Edge
				  R1
		L2   L1   R2
		| -- | -- |
		rect.left < left < right == rect.right

		s4: Outside
		L2   L1   R1   R2
		| -- | -- | -- |

		rect.left < left < right < rect.right

		s5: Crosses Both Edges
		L1   L2   R2   R1
		| -- | -- | -- |
		left < rect.left < rect.right < right

		s6: Crosses Both Edges
		L2   L1   R1   R2
		| -- | -- | -- |
		rect.left < left < right < rect.right

	*/
	
	result.crossover = (
		((left >= rect.left && left < rect.right) || (left < rect.left && right > rect.left)) &&
		((top >= rect.top && top < rect.bottom || top < rect.top && bottom > rect.top == 1))
		);


	if (result.crossover)
	{
		if (leftleft		==  1)	result.left		= sideStatus::crossover;
		if (toptop			==  1)	result.top		= sideStatus::crossover;
		if (rightright		== -1)	result.right	= sideStatus::crossover;
		if (bottombottom	== -1)	result.right	= sideStatus::crossover;
	}





	if (getSharedEdges) {

		if (!leftleft) {
			//result.left = sideStatus::sharedSame;
			result.left = 1;
			result.leftLeft = 1;
		}

		if (!leftright) {
			result.left = 1;
			result.leftRight = 1;
		}

		if (!rightleft) {
			result.right = 1;
			result.rightLeft = 1;
		}

		if (!rightright) {
			result.right = 1;
			result.rightLeft = 1;
		}

		if (!toptop) {
			result.top = 1;
			result.topTop = 1;
		}

		if (!topbottom) {
			result.top = 1;
			result.topBottom = 1;
		}

		if (!bottombottom) {
			result.bottom = 1;
			result.bottomBottom = 1;
		}

		if (!bottomtop) {
			result.bottom = 1;
			result.bottomTop = 1;
		}
	}

	return result;
}


DKGeometry::DPoint::DPoint(const DSize & pt)
{
	x = pt.width;
	y = pt.height;
}


void DKGeometry::DPoint::fix()
{
	float intpart, fractpart;

	fractpart = modff(x, &intpart);
	x = (fractpart > 0.15f) ? intpart + 0.5f : intpart - 0.5f;
	
	fractpart = modff(y, &intpart);
	y = (fractpart > 0.15f) ? intpart + 0.5f : intpart - 0.5f;

}

void DKGeometry::DPoint::boundInRect(DRect rect)
{
	if (x < rect.left) x = rect.left;
	if (x > rect.right) x = rect.right;
	if (y < rect.top) y = rect.top;
	if (y > rect.bottom) y = rect.bottom;
}

DRect DKGeometry::GetCombinedRect(const DRectArray & rectarray)
{
	if (rectarray.size() == 1) { return rectarray.front(); }
	DRect cRect = rectarray.front();
	for (const auto&eachrect : rectarray)
	{
		cRect.CombineWith(eachrect);
	}
	return cRect;
}

DRect DKGeometry::ERROR_RECT()
{
	return DRect(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
}

DRect DKGeometry::INFINITY_RECT()
{
	return DRect(-INFINITY,-INFINITY, INFINITY, INFINITY);
}

bool DKGeometry::test()
{
	DRect testRect(100, 100, 200, 200);
	DLine horizontalLine(DPoint(0,100), DPoint(100, 100));
	DLine horizontalLine2(DPoint(0, 150), DPoint(100, 150));
	DLine infHorizLine = DLine::horizontalLine(100);
	DLine infVertLine = DLine::verticalLine(100);
	DLine verticalLine(DPoint(50, 0), DPoint(50, 200));
	DLine verticalLine2(DPoint(100, 0), DPoint(100, 200));
	DLine slantLine1(DPoint(0, 0), DPoint(200, 200));
	DLine slantLine2(DPoint(0, 200), DPoint(200, 0));
	DLine slantLine3(DPoint(10, 10), DPoint(210,210));
	ASSERT(testRect.test(110, 110, 190, 210)); // bottom overlap
	ASSERT(testRect.test(110, 90, 190, 210)); // top & bottom overlap
	ASSERT(testRect.test(90, 90, 110, 210)); // top & bottom & left overlap
	ASSERT(testRect.test(90, 110, 110, 210)); // bottom & left overlap
	ASSERT(testRect.test(90, 110, 210, 190)); // left & right overlap
	ASSERT(testRect.test(190, 110, 210, 210)); // bottom & right overlap
	ASSERT(testRect.test(110, 110, 210, 190)); // right overlap
	ASSERT(testRect.test(190, 90, 210, 110)); // top & right overlap
	ASSERT(testRect.test(110, 90, 190, 110)); // top overlap
	ASSERT(testRect.test(90, 90, 110, 110)); // top & left overlap
	ASSERT(testRect.test(90, 110, 110, 190)); // left overlap
	ASSERT(testRect.test(110, 110, 190, 190)); // inside
	ASSERT(testRect.test(90, 90, 210, 210)); // outside
	ASSERT(testRect.test(100, 110, 190, 210)); // left=left bottom overlap
	ASSERT(testRect.test(100, 90, 190, 210)); // left=left top &  bottom overlap
	ASSERT(testRect.test(100, 90, 190, 190)); // left=left top overlap
	ASSERT(testRect.test(100, 110, 210, 190)); // left=left right overlap
	ASSERT(testRect.test(100, 190, 210, 210)); // left = left right & bottom overlap
	ASSERT(testRect.test(100, 90, 210, 110)); // left = left right & top overlap
	ASSERT(testRect.test(100, 90, 200, 110)); // left = left right = right top overlap
	ASSERT(testRect.test(100, 190, 200, 210)); // left=left right=right bottom overlap
	ASSERT(testRect.test(100, 90, 200, 210)); // left=left right=right top & bottom overlap
	ASSERT(testRect.test(10, 10, 20, 20) == false); // outside
	ASSERT(testRect.test(10, 10, 100, 20) == false); // outside right=right
	ASSERT(testRect.test(10, 10, 100, 200) == false); // outside right=right
	ASSERT(testRect.test(100, 210, 200, 250) == false); // outside
	ASSERT(testRect.test(100, 100, 200, 200)); // same rect
	ASSERT(testRect.test(110, 110, 200, 210)); // right=right bottom overlap
	ASSERT(testRect.test(90, 90, 200, 210)); // right=right left & top & bottom overlap
	ASSERT(testRect.test(110, 90, 200, 210)); // right=right top & bottom overlap
	ASSERT(testRect.IsContainedIn(DKGeometry::INFINITY_RECT()));
	ASSERT(testRect.Intersects(DKGeometry::INFINITY_RECT()));

	// Line Tests
	ASSERT(slantLine1.crosses(slantLine2)); // test obvious line cross
	ASSERT(horizontalLine.crosses(slantLine1));
	ASSERT(horizontalLine.crosses(verticalLine));
	ASSERT(horizontalLine.crosses(slantLine2));
	ASSERT(!horizontalLine.crosses(horizontalLine2));
	ASSERT(verticalLine.crosses(slantLine1));
	ASSERT(verticalLine.crosses(slantLine2));
	ASSERT(!verticalLine.crosses(verticalLine2));
	ASSERT(!slantLine1.crosses(slantLine3));
	ASSERT(!infHorizLine.crosses(horizontalLine));
	ASSERT(!horizontalLine.crosses(infHorizLine));
	ASSERT(infHorizLine.crosses(verticalLine));
	ASSERT(verticalLine.crosses(infHorizLine));
	ASSERT(infHorizLine.crosses(infVertLine));

	return false;
}



DKGeometry::DSize::operator DPoint()
{
	return DPoint(width, height);
}
