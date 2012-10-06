#include <string>
#include <vector>

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

// orientation
typedef enum {eLeft = 1, eRight = 2, eUp = 4, eDown = 8} orient_t;
// alignment can be OR'd of zero or more orientations.
typedef unsigned char align_t;
// BGR color
typedef long color_t;
// screen units
typedef long meter_t;
// font style
typedef char style_t;
// object identity
typedef long identity_t;
// handle 
typedef void* handle_t;

#ifdef _UNICODE
// std::wstring for UTF-16
typedef std::wstring string_t;
#else
// std::string for UTF-8
typedef std::string string_t;
#endif

// 2D canvas coordinate
typedef struct Point
{
	// placement as x,y coordinates
	meter_t x, y;
}
point_t;

// 2D rectangular area
typedef struct Area
{
	// area as width and height.
	meter_t wide, high;
}
area_t;

// rectangular camvas area.
typedef struct Rectangle
{
	// placement as x,y coordinates (top-left)
	meter_t x, y;
	// width and height.
	meter_t wide, high;

	// four sides
	meter_t left(bool inside = false) const;
	meter_t top(bool inside = false) const;
	meter_t right(bool inside = false) const;
	meter_t bottom(bool inside = false) const;

	// four corners.
	point_t topLeft(bool inside = false) const;
	point_t topRight(bool inside = false) const;
	point_t bottomLeft(bool inside = false) const;
	point_t bottomRight(bool inside = false) const;

	area_t getArea() const;
	void setArea(const area_t &pt);
	point_t getPoint() const;
	void setPoint(const point_t &pt);

	// compute intersection of two rectangles.
	// returns true if rectangles overlap.
	bool intersect(const Rectangle &left, const Rectangle &right);

	// Shrink rectangle equal amount along all four sides.
	// returns false if resulting rectangle has zero or negative area.
	bool shrink(meter_t border);

	// polyline (five points)
	bool polyline(std::vector<point_t> &list, bool inside = true);

	// clip a polyline
	void clipPolyline(const std::vector<point_t> &list, std::vector< std::vector<point_t> > &lines);

	bool equals(const Rectangle &right) const;
}
rect_t;

orient_t orient(const std::string &text);
bool orient(std::string &text, orient_t flow);

};