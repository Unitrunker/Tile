#include "stdafx.h"
#include "CanvasProxy.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

CanvasProxy::CanvasProxy(ICanvas *proxy, FILE *file) : _proxy(proxy), _file(file)
{
}

// accessors for bounding canvas rectangle.
bool CanvasProxy::setPlace(const rect_t &rect)
{
	bool bOK = _proxy->setPlace(rect);
	fprintf(_file, "%s(%d,%d,%d,%d) returns %d\n", "setPlace", rect.x, rect.y, rect.wide, rect.high, bOK);
	return bOK;
}

bool CanvasProxy::getPlace(rect_t &rect) const
{
	bool bOK = _proxy->getPlace(rect);
	fprintf(_file, "%s(%d,%d,%d,%d) returns %d\n", "getPlace", rect.x, rect.y, rect.wide, rect.high, bOK);
	return bOK;
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="orient">orientation</param>
/// <param name="text">text string to display in the designated font.</param>
void CanvasProxy::DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, string_t &text)
{
	_proxy->DrawString(rect, box, fore, back, font, align, text);
	fprintf(_file, "%s(%d,%d,%d,%d,%08lX,%08lX,%d)\n", "DrawString", 
		rect.x, rect.y, rect.wide, rect.high, fore, back, align);
}
/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="text">text string to display in the designated font.</param>
/// <param name="cursor">cursor recticle placement</param>
void CanvasProxy::DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, string_t &text, size_t cursor)
{
	_proxy->DrawEditString(rect, box, fore, back, font, align, text, cursor);
	fprintf(_file, "%s(%d,%d,%d,%d,%08lX,%08lX,%d)\n", "DrawEditString", 
		rect.x, rect.y, rect.wide, rect.high, fore, back, cursor);
}
/// <param name="rect">bounding rectangle</param>
/// <param name="fill">polygon fill color</param>
void CanvasProxy::FillRectangle(const rect_t &rect, color_t fill)
{
	_proxy->FillRectangle(rect, fill);
	fprintf(_file, "%s(%d,%d,%d,%d,%08lX)\n", "FillRectangle", rect.x, rect.y, rect.wide, rect.high, fill);
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
void CanvasProxy::DrawPolyline(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color)
{
	_proxy->DrawPolyline(rect, box, list, count, width, color);
	fprintf(_file, "%s(", "DrawPolyline");
	for (size_t i = 0; i < count; i++)
		fprintf(_file, "(%d,%d)", list[i].x, list[i].y);
	fprintf(_file, "%d,%d,%08lX)\n", count, width, color);
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
/// <param name="fill">polygon fill color</param>
void CanvasProxy::DrawPolygon(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color, color_t fill)
{
	_proxy->DrawPolygon(rect, box, list, count, width, color, fill);
	fprintf(_file, "%s(", "DrawPolygon");
	for (size_t i = 0; i < count; i++)
		fprintf(_file, "(%d,%d)", list[i].x, list[i].y);
	fprintf(_file, "%d,%d,%08lX,%08lX)\n", count, width, color, fill);
}
