#include "stdafx.h"
#include "MockCanvas.h"

/*
Copyright 2011 Rick Parrish
*/

// accessors for bounding canvas rectangle.
bool MockCanvas::setPlace(const rect_t &)
{
	printf("%s\n", __FUNCTION__);
	return false;
}

bool MockCanvas::getPlace(rect_t &) const
{
	printf("%s\n", __FUNCTION__);
	return false;
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="orient">orientation</param>
/// <param name="text">text string to display in the designated font.</param>
void MockCanvas::DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
{
	printf("%s\n", __FUNCTION__);
	rect;
	fore;
	back;
	font;
	align;
	text;
}
/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="text">text string to display in the designated font.</param>
/// <param name="cursor">cursor reticle.</param>
void MockCanvas::DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text, size_t cursor)
{
	printf("%s\n", __FUNCTION__);
	rect;
	fore;
	back;
	font;
	align;
	text;
	cursor;
}
/// <param name="rect">bounding rectangle</param>
/// <param name="fill">polygon fill color</param>
void MockCanvas::FillRectangle(const rect_t &rect, color_t fill)
{
	printf("%s\n", __FUNCTION__);
	rect;
	fill;
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
void MockCanvas::DrawPolyline(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color)
{
	printf("%s\n", __FUNCTION__);
	list;
	count;
	width;
	color;
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
/// <param name="fill">polygon fill color</param>
void MockCanvas::DrawPolygon(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color, color_t fill)
{
	printf("%s\n", __FUNCTION__);
	list;
	count;
	width;
	color;
	fill;
}
// true if overlaps update region.
bool MockCanvas::overlaps(rect_t&)
{
	printf("%s\n", __FUNCTION__);
	return true;
}
