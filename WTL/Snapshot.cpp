#include "stdafx.h"
#include "Snapshot.h"
#include <vector>
#include <atlimage.h>
#include <Gdiplusimaging.h>
#include "Cache.h"

/*
Copyright 2011 Rick Parrish
*/

Snapshot::Snapshot(HWND hWnd)
{
	RECT rect = {0};
	GetClientRect(hWnd, &rect);
	_clip.wide = rect.right;
	_clip.high = rect.bottom;

	HDC hDC = GetDC(hWnd);

	CreateCompatibleDC(hDC);
	_bmp.CreateCompatibleBitmap(hDC, _clip.wide, _clip.high);
	SelectBitmap(_bmp);
}

// accessors for bounding canvas rectangle.
bool Snapshot::setPlace(const rect_t &clip)
{
	_clip = clip;
	return true;
}

bool Snapshot::getPlace(rect_t &rect) const
{
	rect = _clip;
	return true;
}

void Snapshot::privateDrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
{
	UINT ta = 0;
	point_t pt = {0};
	RECT rc = {0};
	SIZE size = {0};

	HFONT hFont = makeFont(font);
	SelectFont(hFont);
	SetTextColor(fore);
	SetBkColor(back);

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
		pt.x = box.wide;
		pt.y = 0;
		ta = TA_RIGHT;
	}
	SetTextAlign(ta);
	rc.right = rect.wide + rect.x;
	rc.bottom = rect.high + rect.y;
	pt.x += rect.x;
	pt.y += rect.y;
	rc.left = rect.x;
	rc.top = rect.y;
	ExtTextOut(pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, &rc, text.c_str(), text.size(), 0);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="orient">orientation</param>
/// <param name="text">text string to display in the designated font.</param>
void Snapshot::DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
{
	privateDrawString(rect, box, fore, back, font, align, text);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fore">foreground text color</param>
/// <param name="back">background color</param>
/// <param name="font">font including face, point size, and style.</param>
/// <param name="text">text string to display in the designated font.</param>
/// <param name="cursor">cursor reticle.</param>
void Snapshot::DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text, size_t cursor)
{
	RECT rc = {0};
	privateDrawString(rect, box, fore, back, font, align, text);
	if (cursor == 0)
	{
		DrawText(_T("A"), 1, &rc, DT_CALCRECT|DT_SINGLELINE);
		rc.right = rc.left;
	}
	else
		DrawText(&text[0], cursor, &rc, DT_CALCRECT|DT_SINGLELINE);
	// zero height?
	if (rc.top == rc.bottom)
		rc.bottom = box.high; // yes: zap for cursor bar.

	rc.top += rect.y;
	rc.bottom += rect.y;
	rc.right += rect.x;

	// draw cursor
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, fore);
	SelectPen(pen);
	// TODO: code to clip line segments.
	MoveTo(rc.right, rc.top);
	LineTo(rc.right, rc.bottom);
}

/// <param name="rect">bounding rectangle</param>
/// <param name="fill">polygon fill color</param>
void Snapshot::FillRectangle(const rect_t &rect, color_t fill)
{
	FillSolidRect(rect.x, rect.y, rect.wide, rect.high, fill);
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
void Snapshot::DrawPolyline(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color)
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
void Snapshot::DrawPolygon(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color, color_t fill)
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

bool Snapshot::overlaps(rect_t &control)
{
	rect_t ignore = {0};
	return ignore.intersect(control, _clip);
}

void Snapshot::Save(const TCHAR *path)
{
	CImage image;
	image.Attach(_bmp);
	image.Save(path);
	image.Detach();
}
