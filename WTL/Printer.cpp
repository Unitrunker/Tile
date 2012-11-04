#include "stdafx.h"
#include "Printer.h"
#include <vector>
#include "Cache.h"

/*
Copyright 2012 Rick Parrish
*/

Printer::Printer(HDC hDC) : CDC(hDC), _iPage(0), _iJob(0)
{
	_di.cbSize = sizeof _di;
	_di.fwType = 0;
	_di.lpszDatatype = 0;
	_di.lpszDocName = 0;
	_di.lpszOutput = 0;
	_clip.x = GetDeviceCaps(PHYSICALOFFSETX);
	_clip.y = GetDeviceCaps(PHYSICALOFFSETY);
	_clip.wide = GetDeviceCaps(PHYSICALWIDTH) - 2*_clip.x;
	_clip.high = GetDeviceCaps(PHYSICALHEIGHT) - 2*_clip.y;
	_dc.CreateCompatibleDC(hDC);
}

// Accessors for bounding canvas rectangle.
bool Printer::setPlace(const rect_t &rect)
{
	rect;
	return false; //MoveWindow(m_hWnd, rect.x, rect.y, rect.wide, rect.high, FALSE) != NULL;
}

bool Printer::getPlace(rect_t &rect) const
{
	rect;
	return false;
	//RECT rc = {0};
	//bool bOK = GetClientRect(m_hWnd, &rc) != 0;
	//if (bOK)
	//{
	//	rect.x = rc.left;
	//	rect.y = rc.top;
	//	rect.wide = rc.right - rc.left;
	//	rect.high = rc.bottom - rc.top;
	//}
	//return bOK;
}

void Printer::privateDrawString(const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
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
void Printer::DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text)
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
void Printer::DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text, size_t cursor)
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
void Printer::FillRectangle(const rect_t &rect, color_t fill)
{
	FillSolidRect(rect.x, rect.y, rect.wide, rect.high, fill);
}
/// <param name="list">list of points</param>
/// <param name="count">number of points</param>
/// <param name="width">pen width for drawing line</param>
/// <param name="color">pen color</param>
void Printer::DrawPolyline(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color)
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
void Printer::DrawPolygon(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color, color_t fill)
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

bool Printer::overlaps(rect_t &control)
{
	rect_t ignore = {0};
	return ignore.intersect(control, _clip);
}

// Start print job.
bool Printer::StartDoc()
{
	_iPage = 0;
	_iJob = ::StartDoc(m_hDC, &_di );
	return _iJob > 0;
}

// Render a single page.
// Use a printer friendly theme eg. high contrast for black & white printers.
bool Printer::DrawPage(ITile *pPage)
{
	// TODO: make page # available via an accessor for header / footer controls 
	// that want to print the page number.
	_iPage++;
	::StartPage(m_hDC);
	pPage->setRect(_clip);
	pPage->Draw(this, false);
	::EndPage(m_hDC);
	return true;
}

// end print job.
bool Printer::EndDoc()
{
	return ::EndDoc(m_hDC) > 0;
}

// cancel print job.
bool Printer::Cancel()
{
	// TODO: need the printer handle to cancel the job.
	return false;
}

#if 0
static bool print(ITile *pDraw)
{
	PRINTDLG pd = {0};
	pd.lStructSize = sizeof pd;

	/* 
	 * get rid of PD_RETURNDEFAULT on the line below if you'd like to
	 * see the "Printer Settings" dialog!
	 *
	 */
	pd.Flags = PD_RETURNDEFAULT | PD_RETURNDC;

	// try to retrieve the printer DC
	if( !PrintDlg(&pd) )
	{
		return false;
	}

	DOCINFO di = {0};
	HDC hPrinter = pd.hDC;

	// initialization of the printing!

	// start the first and only page

	Printer canvas(hPrinter);

	RECT rect = {0};
	canvas.GetClipBox(&rect);
	Tiles::rect_t wrect;
	wrect.x = rect.left;
	wrect.y = rect.top;
	wrect.wide = rect.right - rect.left;
	wrect.high = rect.bottom - rect.top;
	pDraw->setRect(wrect);

	pDraw->Draw(&canvas, false);

	// uncomment the following line to print in colour! :)
	// SetTextColor( hPrinter, 0x0000FF );

	// finish the first page
	EndPage( hPrinter );

	// end this document and release the printer's DC
	EndDoc( hPrinter );
	//DeleteDC( hPrinter );
	return true;
}
#endif