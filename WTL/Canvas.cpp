#include "stdafx.h"
#include "Canvas.h"
#include <vector>
#include "Cache.h"

/*
Copyright 2011 Rick Parrish
*/

Canvas::Canvas(HWND hWnd) : CPaintDC(hWnd)
{
	_clip.x = m_ps.rcPaint.left;
	_clip.y = m_ps.rcPaint.top;
	_clip.wide = m_ps.rcPaint.right - _clip.x;
	_clip.high = m_ps.rcPaint.bottom - _clip.y;
	_dc.CreateCompatibleDC(m_hDC);
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

void Canvas::privateDrawString(const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
{
	UINT ta = 0;
	point_t pt = {0};
	RECT rc = {0};
	SIZE size = {0};

	if ( _bmp.IsNull() )
	{
		_bmp.CreateCompatibleBitmap(m_hDC, box.wide, box.high);
	}
	else if ( _bmp.GetSize(size) && (size.cx < box.wide || size.cy < box.high) )
	{
		_bmp.DeleteObject();
		_bmp.CreateCompatibleBitmap(m_hDC, box.wide, box.high);
	}

	_dc.SelectBitmap(_bmp);
	HFONT hFont = makeFont(font);
	_dc.SelectFont(hFont);
	_dc.SetTextColor(fore);
	_dc.SetBkColor(back);

	if ( (align & eLeft) && (align & eRight) )
	{
		pt.x = box.wide / 2;
		pt.y = 0;
		ta = TA_CENTER;
	}
	else if (align & eLeft)
	{
		pt.x = 0;
		pt.y = 0;
		ta = TA_LEFT;
	}
	else if (align & eRight)
	{
		pt.x = box.x + box.wide;
		pt.y = 0;
		ta = TA_RIGHT;
	}
	_dc.SetTextAlign(ta);
	rc.right = box.wide;
	rc.bottom = box.high;
	_dc.ExtTextOut(pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, &rc, text.c_str(), text.size(), 0);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="orient">orientation</param>
/// <param name="text">text string to display in the designated font.</param>
void Canvas::DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
{
	privateDrawString(box, fore, back, font, align, text);
	BitBlt(rect.x, rect.y, rect.wide, rect.high, _dc, box.x, box.y, SRCCOPY);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="text">text string to display in the designated font.</param>
/// <param name="cursor">cursor reticle.</param>
void Canvas::DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text, size_t cursor)
{
	RECT rc = {0};
	privateDrawString(box, fore, back, font, align, text);
	if (cursor == 0)
	{
		_dc.DrawText(_T("A"), 1, &rc, DT_CALCRECT|DT_SINGLELINE);
		rc.right = rc.left;
	}
	else
		_dc.DrawText(&text[0], cursor, &rc, DT_CALCRECT|DT_SINGLELINE);
	// zero height?
	if (rc.top == rc.bottom)
		rc.bottom = box.high; // yes: zap for cursor bar.

	// draw cursor
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, fore);
	_dc.SelectPen(pen);
	// TODO: code to clip line segments.
	_dc.MoveTo(rc.right, rc.top);
	_dc.LineTo(rc.right, rc.bottom);

	BitBlt(rect.x, rect.y, rect.wide, rect.high, _dc, box.x, box.y, SRCCOPY);
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

	for (size_t i = 0; i < count; i++)
	{
		pt.x = list[i].x + rect.x - box.x;
		pt.y = list[i].y + rect.y - box.y;
		pts.push_back(pt);
	}
	pen.CreatePen(PS_SOLID, width, color);
	HPEN hSave = SelectPen(pen);

	CRgn region;
	region.CreateRectRgn(rect.x, rect.y, rect.x + rect.wide, rect.y + rect.high);
	SelectClipRgn(region);

	Polyline(&pts[0], pts.size());
	SelectPen(hSave);
	SelectClipRgn(NULL);
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

	for (size_t i = 0; i < count; i++)
	{
		pt.x = list[i].x - box.x + rect.x;
		pt.y = list[i].y - box.y + rect.y;
		pts.push_back(pt);
	}
	CRgn region;
	region.CreateRectRgn(rect.x, rect.y, rect.x + rect.wide, rect.y + rect.high);
	SelectClipRgn(region);

	brush.CreateSolidBrush(fill);
	pen.CreatePen(PS_SOLID, width, color);
	HPEN hSavePen = SelectPen(pen);
	HBRUSH hSaveBrush = SelectBrush(brush);
	Polygon(&pts[0], pts.size());

	SelectPen(hSavePen);
	SelectBrush(hSaveBrush);
	SelectClipRgn(NULL);
}

bool Canvas::overlaps(rect_t &control)
{
	rect_t ignore = {0};
	return ignore.intersect(control, _clip);
}

void Canvas::GradientFill(const rect_t &area, const rect_t &box, char flow, color_t nr, color_t fr)
{
	TRIVERTEX list[4] = {0};
	GRADIENT_RECT span = {0};
	list[0].x = 0;
	list[0].y = 0;
	list[1].x = box.wide;
	list[1].y = 0;
	list[2].x = box.wide;
	list[2].y = box.high;
	list[0].x = 0;
	list[0].y = box.high;
	span.LowerRight = nr;
	span.UpperLeft = fr;
	_dc.GradientFill(list, _countof(list), &span, 1, GRADIENT_FILL_RECT_V);	
}
