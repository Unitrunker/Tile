#include "stdafx.h"
#include "Types.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

// left side
meter_t Rectangle::left(bool) const
{
	return x;
}

// top side
meter_t Rectangle::top(bool) const
{
	return y;
}

// right side
meter_t Rectangle::right(bool inside) const
{
	meter_t right = x + wide;
	if (inside)
		right--;
	return right;
}

// bottom side
meter_t Rectangle::bottom(bool inside) const
{
	meter_t bottom = y + high;
	if (inside)
		bottom--;
	return bottom;
}

// top, left corner
point_t Rectangle::topLeft(bool inside) const
{
	point_t pt = {left(inside), top(inside)};
	return pt;
}

// top, right corner
point_t Rectangle::topRight(bool inside) const
{
	point_t pt = {right(inside), top(inside)};
	return pt;
}

// bottom, left corner
point_t Rectangle::bottomLeft(bool inside) const
{
	point_t pt = {left(inside), bottom(inside)};
	return pt;
}

// bottom, right corner
point_t Rectangle::bottomRight(bool inside) const
{
	point_t pt = {right(inside), bottom(inside)};
	return pt;
}

// Evaluate intersecting line segments.
// Return true if segments overlap.
// (a,b) is line segment common to left/right params.
static bool line_intersect(meter_t &a, meter_t &b, 
	meter_t lefta, meter_t leftb, meter_t righta, meter_t rightb)
{
	// negative line segments are not allowed.
	if (leftb < lefta)
		return false;

	// negative line segments are not allowed.
	if (rightb < righta)
		return false;

	// right before left and non-overlapping?
	if (rightb < lefta)
		return false;

	// left before right and non-overlapping?
	if (leftb < righta)
		return false;

	// left "a" lies between right's (a,b)?
	if (lefta >= righta && lefta <= rightb)
		// yes
		a = lefta;
	else
		// no
		a = righta;

	// left "b" lies between right's (a,b)?
	if (leftb >= righta && leftb <= rightb)
		// yes
		b = leftb;
	else
		// no
		b = rightb;

	return a < b;
}

// Compute intersecting rectangle.
// Returns true if A and B overlap.
// If false, resulting rectangle is undefined.
bool Rectangle::intersect(const Rectangle &a, const Rectangle &b)
{
	meter_t bottom = 0, right = 0;

	// compare horzontal sides of the rectangles
	// evaluate as intersecting line segments.
	bool bH = line_intersect(x, right, a.x, a.right(), b.x, b.right());

	// compare vertical sides of the rectangles.
	// evaluate as intersecting line segments.
	bool bV = line_intersect(y, bottom, a.y, a.bottom(), b.y, b.bottom());

	wide = right - x;
	high = bottom - y;

	return bH && bV;
}

// Shrink rectangle equal amount along all four sides.
// returns false if resulting rectangle has zero or negative area.
bool Rectangle::shrink(meter_t border)
{
	x += border;
	y += border;
	high -= border*2;
	wide -= border*2;
	return high > 0 && wide > 0;
}

bool Rectangle::polyline(std::vector<point_t> &list, bool inside)
{
	list.push_back(topLeft(inside));
	list.push_back(topRight(inside));
	list.push_back(bottomRight(inside));
	list.push_back(bottomLeft(inside));
	list.push_back(topLeft(inside));
	return false;
}

static void clipLeft(meter_t left, const std::vector<point_t> &list, std::vector< std::vector<point_t> > &lines)
{
	size_t i = 0;
	bool inside = false;
	std::vector<point_t> line;

	point_t last;
	bool was = false;
	while (i < list.size() )
	{
		point_t pt = list[i];
		bool is = pt.x >= left;	// inside
		if (i > 0 && was ^ is)
		{
			// extrapolate
			point_t deltas = {0};
			deltas.x = (pt.x - last.x);
			deltas.y = (pt.y - last.y);
			last.y = last.y + deltas.y * (left - last.x) / deltas.x;
			last.x = left;
			line.push_back(last);
		}
		if (is)
		{
			line.push_back(pt);
		}
		else if (was)
		{
			lines.push_back(line);
			line.clear();
		}
		was = is;
		last = pt;
		i++;
	}
	if (line.size())
		lines.push_back(line);
}

static void clipRight(meter_t right, const std::vector<point_t> &list, std::vector< std::vector<point_t> > &lines)
{
	size_t i = 0;
	bool inside = false;
	std::vector<point_t> line;

	point_t last;
	bool was = false;
	while (i < list.size() )
	{
		point_t pt = list[i];
		bool is = pt.x < right;	// inside
		if (i > 0 && was ^ is)
		{
			// extrapolate
			point_t deltas = {0};
			deltas.x = (pt.x - last.x);
			deltas.y = (pt.y - last.y);
			last.y = last.y + deltas.y * (right - last.x) / deltas.x;
			last.x = right;
			line.push_back(last);
		}
		if (is)
		{
			line.push_back(pt);
		}
		else if (was)
		{
			lines.push_back(line);
			line.clear();
		}
		was = is;
		last = pt;
		i++;
	}
	if (line.size())
		lines.push_back(line);
}

static void clipTop(meter_t top, const std::vector<point_t> &list, std::vector< std::vector<point_t> > &lines)
{
	size_t i = 0;
	bool inside = false;
	std::vector<point_t> line;

	point_t last;
	bool was = false;
	while (i < list.size() )
	{
		point_t pt = list[i];
		bool is = pt.y >= top;	// inside
		if (i > 0 && was ^ is)
		{
			// extrapolate
			point_t deltas = {0};
			deltas.x = (pt.x - last.x);
			deltas.y = (pt.y - last.y);
			last.x = last.x + deltas.x * (top - last.y) / deltas.y;
			last.y = top;
			line.push_back(last);
		}
		if (is)
		{
			line.push_back(pt);
		}
		else if (was)
		{
			lines.push_back(line);
			line.clear();
		}
		was = is;
		last = pt;
		i++;
	}
	if (line.size())
		lines.push_back(line);
}

static void clipBottom(meter_t bottom, const std::vector<point_t> &list, std::vector< std::vector<point_t> > &lines)
{
	size_t i = 0;
	bool inside = false;
	std::vector<point_t> line;

	point_t last;
	bool was = false;
	while (i < list.size() )
	{
		point_t pt = list[i];
		bool is = pt.y < bottom;	// inside
		if (i > 0 && was ^ is)
		{
			// extrapolate
			point_t deltas = {0};
			deltas.x = (pt.x - last.x);
			deltas.y = (pt.y - last.y);
			last.x = last.x + deltas.x * (bottom - last.y) / deltas.y;
			last.y = bottom;
			line.push_back(last);
		}
		if (is)
		{
			line.push_back(pt);
		}
		else if (was)
		{
			lines.push_back(line);
			line.clear();
		}
		was = is;
		last = pt;
		i++;
	}
	if (line.size())
		lines.push_back(line);
}

// clip a polyline
void Rectangle::clipPolyline(const std::vector<point_t> &list, std::vector< std::vector<point_t> > &lines)
{
	std::vector< std::vector<point_t> > work, result;
	work.push_back(list);

	// clip along the left border
	for (size_t i = 0; i < work.size(); i++)
	{
		std::vector< std::vector<point_t> > play;
		clipLeft(left(), work[i], play);
		result.insert(result.end(), play.begin(), play.end());
	}
	work.assign(result.begin(), result.end());
	result.clear();
	// clip along the right border
	for (size_t i = 0; i < work.size(); i++)
	{
		std::vector< std::vector<point_t> > play;
		clipRight(right(), work[i], play);
		result.insert(result.end(), play.begin(), play.end());
	}
	work.assign(result.begin(), result.end());
	result.clear();
	// clip along the top border
	for (size_t i = 0; i < work.size(); i++)
	{
		std::vector< std::vector<point_t> > play;
		clipTop(top(), work[i], play);
		result.insert(result.end(), play.begin(), play.end());
	}
	work.assign(result.begin(), result.end());
	result.clear();
	// clip along the bottom border
	for (size_t i = 0; i < work.size(); i++)
	{
		std::vector< std::vector<point_t> > play;
		clipBottom(bottom(), work[i], play);
		result.insert(result.end(), play.begin(), play.end());
	}
	lines.assign(result.begin(), result.end());
}

bool Rectangle::equals(const Rectangle &right) const
{
	return x == right.x && y == right.y && wide == right.wide && high == right.high;
}

area_t Rectangle::getArea() const
{
	area_t area = {wide, high};
	return area;
}

void Rectangle::setArea(const area_t &area)
{
	wide = area.wide;
	high = area.high;
}

// same as topLeft();
point_t Rectangle::getPoint() const
{
	point_t pt = {x, y};
	return pt;
}

void Rectangle::setPoint(const point_t &pt)
{
	x = pt.x;
	y = pt.y;
}

static bool flag(unsigned char &work, const std::string &text, size_t &offset, orient_t flow, const char *tag)
{
	size_t len = strlen(tag);
	if ( text.compare(offset, len, tag) == 0 )
	{
		work |= flow;
		offset += len;
		return true;
	}
	return false;
}

orient_t orient(const std::string &text)
{
	unsigned char flow = (orient_t)0;
	size_t offset = 0;
	do
	{
		// lazy eval: first one wins
		flag(flow, text, offset, eUp, "up") ||
			flag(flow, text, offset, eDown, "down") ||
			flag(flow, text, offset, eLeft, "left") ||
			flag(flow, text, offset, eRight, "right");
	}
	while (text.compare(offset++, 1, "|") == 0);
	return (orient_t)flow;
}

static void flag(std::string &text, unsigned char &work, orient_t flow, const char *tag)
{
	if (work & flow)
	{
		if (text.size())
			text.append("|");
		text.append(tag);
		work &= ~flow;
	}
}

bool orient(std::string &text, orient_t flow)
{
	unsigned char work = flow;
	flag(text, work, eUp, "up");
	flag(text, work, eDown, "down");
	flag(text, work, eLeft, "left");
	flag(text, work, eRight, "right");
	return work == 0;
}

};
