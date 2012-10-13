#include "stdafx.h"
#include "Canvas.h"
#include <vector>
#include "Cache.h"

/*
Copyright 2011 Rick Parrish
*/

Canvas::Canvas(CWnd *pWindow) : CPaintDC(pWindow)
{
	_clip.x = m_ps.rcPaint.left;
	_clip.y = m_ps.rcPaint.top;
	_clip.wide = m_ps.rcPaint.right - _clip.x;
	_clip.high = m_ps.rcPaint.bottom - _clip.y;
}

// accessors for bounding canvas rectangle.
bool Canvas::setPlace(const rect_t &rect)
{
	return MoveWindow(m_hWnd, rect.x, rect.y, rect.wide, rect.high, FALSE) != NULL;
}

bool Canvas::getPlace(rect_t &rect) const
{
	RECT rc = {0};
	bool bOK = GetClientRect(m_hWnd, &rc) != 0;
	if (bOK)
	{
		rect.x = rc.left;
		rect.y = rc.top;
		rect.wide = rc.right - rc.left;
		rect.high = rc.bottom - rc.top;
	}
	return bOK;
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="orient">orientation</param>
/// <param name="text">text string to display in the designated font.</param>
void Canvas::DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Tiles::Font &font, align_t align, const string_t &text)
{
	RECT rc = {0};
	rc.left = rect.x;
	rc.right = rect.x + rect.wide;
	rc.top = rect.y;
	rc.bottom = rect.y + rect.high;

	box; // TODO

	HGDIOBJ hFont = static_cast<HGDIOBJ>( makeFont(font) );
	hFont = SelectObject(hFont);
	SetTextColor(fore);
	SetBkColor(back);
	UINT ta = 0;
	point_t pt = {0};
	if ( (align & eLeft) && (align & eRight) )
	{
		pt.x = rect.x + rect.wide / 2;
		pt.y = rect.y;
		ta = TA_CENTER;
	}
	else if (align & eLeft)
	{
		pt.x = rect.x;
		pt.y = rect.y;
		ta = TA_LEFT;
	}
	else if (align & eRight)
	{
		pt.x = rect.x + rect.wide;
		pt.y = rect.y;
		ta = TA_RIGHT;
	}
	SetTextAlign(ta);
	ExtTextOut(pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, &rc, text.c_str(), text.size(), 0);
	SelectObject(hFont);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="text">text string to display in the designated font.</param>
/// <param name="cursor">cursor reticle.</param>
void Canvas::DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Tiles::Font &font, align_t align, const string_t &text, size_t cursor)
{
	RECT rc = {0};
	rc.left = rect.x;
	rc.right = rect.x + rect.wide;
	rc.top = rect.y;
	rc.bottom = rect.y + rect.high;

	box; // TODO

	HGDIOBJ hFont = static_cast<HGDIOBJ>( makeFont(font) );
	hFont = SelectObject(hFont);
	SetTextColor(fore);
	SetBkColor(back);
	UINT ta = 0;
	point_t pt = {0};
	if ( (align & eLeft) && (align & eRight) )
	{
		pt.x = rect.x + rect.wide / 2;
		pt.y = rect.y;
		ta = TA_CENTER;
	}
	else if (align & eLeft)
	{
		pt.x = rect.x;
		pt.y = rect.y;
		ta = TA_LEFT;
	}
	else if (align & eRight)
	{
		pt.x = rect.x + rect.wide;
		pt.y = rect.y;
		ta = TA_RIGHT;
	}
	SetTextAlign(ta);
	ExtTextOut(pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, &rc, text.c_str(), text.size(), 0);

	if (cursor == 0)
	{
		DrawText(_T("A"), 1, &rc, DT_CALCRECT|DT_SINGLELINE);
		rc.right = rc.left;
	}
	else
		DrawText(&text[0], cursor, &rc, DT_CALCRECT|DT_SINGLELINE);
	// zero height?
	if (rc.top == rc.bottom)
		rc.bottom = rect.y + rect.high; // yes: zap for cursor bar.

	// draw cursor
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, fore);
	HGDIOBJ hSave = SelectObject(pen);
	MoveTo(rc.right, rc.top);
	LineTo(rc.right, rc.bottom);
	SelectObject(hSave);
	SelectObject(hFont);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fill">polygon fill color</param>
void Canvas::FillRectangle(const rect_t &rect, color_t fill)
{
	FillSolidRect(rect.x, rect.y, rect.wide, rect.high, fill);
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
void Canvas::DrawPolyline(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color)
{
	CPen pen;
	std::vector<POINT> pts;
	POINT pt = {0};

	rect; // TODO
	box; // TODO

	for (size_t i = 0; i < count; i++)
	{
		pt.x = list[i].x;
		pt.y = list[i].y;
		pts.push_back(pt);
	}
	pen.CreatePen(PS_SOLID, width, color);
	HGDIOBJ hSave = SelectObject(pen);
	Polyline(&pts[0], pts.size());
	SelectObject(hSave);
}

/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
/// <param name="fill">polygon fill color</param>
void Canvas::DrawPolygon(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color, color_t fill)
{
	CPen pen;
	CBrush brush;
	std::vector<POINT> pts;
	POINT pt = {0};

	rect; // ignored.
	box;  // ignored.

	for (size_t i = 0; i < count; i++)
	{
		pt.x = list[i].x;
		pt.y = list[i].y;
		pts.push_back(pt);
	}
	brush.CreateSolidBrush(fill);
	pen.CreatePen(PS_SOLID, width, color);
	HGDIOBJ hSavePen = SelectObject(pen);
	HGDIOBJ hSaveBrush = SelectObject(brush);
	Polygon(&pts[0], pts.size());
	SelectObject(hSavePen);
	SelectObject(hSaveBrush);
}

bool Canvas::overlaps(rect_t &control)
{
	rect_t ignore = {0};
	return ignore.intersect(control, _clip);
}
