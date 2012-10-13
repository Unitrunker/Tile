#include "stdafx.h"
#include "Theme.h"
#include "Font.h"
#include "../JSON/Writer.h"
#include "../JSON/Reader.h"
#include "../Stream/FileInputStream.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

static const color_t bk = RGB(0,0,0);
static const color_t gr = RGB(128,128,128);
static const color_t wt = RGB(255,255,255);
static const color_t sl = RGB(192,192,192);

static wchar_t spade = 2660;
static wchar_t club = 2663;
static wchar_t heart = 2665;
static wchar_t diamond = 2666;

static const TCHAR expand[] = {0xFEFF, 0x229E, 0};
static const TCHAR collapse[] = {0xFEFF, 0x229F, 0};

//static const TCHAR checked[] = _T("\xFD");
//static const TCHAR uncheck[] = _T("\x6F");
static const TCHAR checked[] = {0x2612, 0};
static const TCHAR uncheck[] = {0x2610, 0};

static const TCHAR dn[] = {0xFEFF, 0x25BC, 0};
static const TCHAR up[] = {0xFEFF, 0x25B2, 0};
static const TCHAR lt[] = {0xFEFF, 0x25C0, 0};
static const TCHAR rt[] = {0xFEFF, 0x25B6, 0};

static const TCHAR lucida[] = _T("Lucida Sans Unicode");
static const TCHAR arial[] = _T("Arial Unicode MS");

Theme::Theme() :
	CaptionBack(gr),
	CaptionFore(bk),
	DataFore(bk),
	DataBack(sl),
	RowFore(bk),
	RowBack(wt),
	CellFore(bk),
	CellBack(wt),
	ToolFore(bk),
	ToolBack(gr),
	ToolOver(wt),
	GridLines(bk),
	GridThick(2),
	Text(lucida, 18, 0),
	Arrow(lucida, 18, 0),
	ArrowUp(up),
	ArrowDn(dn),
	ArrowLt(lt),
	ArrowRt(rt),
	Checked(checked),
	Uncheck(uncheck),
	Expand(expand),
	Collapse(collapse),
	Stock(_T("Lucinda Sans Unicode"), 18, 0)
{
}

Theme::Theme(const Theme &copy) :
	CaptionBack(copy.CaptionBack),
	CaptionFore(copy.CaptionFore),
	DataBack(copy.DataBack),
	DataFore(copy.DataFore),
	CellFore(copy.CellFore),
	CellBack(copy.CellBack),
	RowFore(copy.RowFore),
	RowBack(copy.RowBack),
	ToolBack(copy.ToolBack),
	ToolFore(copy.ToolFore),
	ToolOver(copy.ToolOver),
	GridLines(copy.GridLines),
	GridThick(copy.GridThick),
	Text(copy.Text),
	Arrow(copy.Arrow),
	ArrowUp(copy.ArrowUp),
	ArrowDn(copy.ArrowDn),
	ArrowLt(copy.ArrowLt),
	ArrowRt(copy.ArrowRt),
	Stock(copy.Stock),
	Checked(copy.Checked),
	Uncheck(copy.Uncheck),
	Expand(copy.Expand),
	Collapse(copy.Collapse)
{
}

const TCHAR *Theme::getName(char index)
{
	static const TCHAR *names[] = 
	{
		_T("Default"),
		_T("CaptionFore"),
		_T("CaptionBack"),
		_T("DataFore"),
		_T("DataBack"),
		_T("SelectedFore"),
		_T("SelectedRowBack"),
		_T("SelectedCellBack"),
		_T("GridLines"),
		_T("ToolFore"),
		_T("ToolBack"),
		_T("ToolOver")
	};

	if (index >= 0 && index < _countof(names))
		return names[index];
	return names[0];
}

bool Theme::save(const TCHAR *path)
{
	JSON::Writer writer;

	if ( writer.Open(path) )
	{
		writer.writeStartNamedObject("Theme");
		writer.writeNamedValue("gridline", GridLines, 16);
		Tiles::Font::save(writer, Text);

		writer.writeStartNamedObject("Caption");
		writer.writeNamedValue("fore", CaptionFore, 16);
		writer.writeNamedValue("back", CaptionBack, 16);
		writer.writeEndObject();

		writer.writeStartNamedObject("Data");
		writer.writeNamedValue("fore", DataFore, 16);
		writer.writeNamedValue("back", DataBack, 16);
		writer.writeEndObject();

		writer.writeStartNamedObject("Cell");
		writer.writeNamedValue("fore", CellFore, 16);
		writer.writeNamedValue("back", CellBack, 16);
		writer.writeEndObject();

		writer.writeStartNamedObject("Row");
		writer.writeNamedValue("fore", RowFore, 16);
		writer.writeNamedValue("back", RowBack, 16);
		writer.writeEndObject();

		writer.writeStartNamedObject("Tool");
		writer.writeNamedValue("fore", ToolFore, 16);
		writer.writeNamedValue("back", ToolBack, 16);
		writer.writeNamedValue("over", ToolOver, 16);
		writer.writeEndObject();

		writer.writeStartNamedObject("Arrow");
		writer.writeNamedValue("up", ArrowUp.c_str());
		writer.writeNamedValue("dn", ArrowDn.c_str());
		writer.writeNamedValue("lt", ArrowLt.c_str());
		writer.writeNamedValue("rt", ArrowRt.c_str());
		Tiles::Font::save(writer, Arrow);
		writer.writeEndObject();

		writer.writeEndObject();
		writer.Close();
		return true;
	}
	return false;
}

static bool tagForeBack(JSON::Reader &json, const char *tag, color_t &fore, color_t &back)
{
	bool bOK = false;
	if ( json.beginNamedObject(tag) )
	{
		do
		{
			bOK = json.namedValue("fore", fore) || json.namedValue("back", back);
		}
		while (bOK && json.comma());
		if (bOK)
			bOK = json.endObject();
	}
	return bOK;
}

static bool tool(Theme& self, JSON::Reader &json)
{
	bool bOK = false;
	if ( json.beginNamedObject("Tool") )
	{
		do
		{
			bOK = json.namedValue("fore", self.ToolFore) ||
				json.namedValue("back", self.ToolBack) ||
				json.namedValue("over", self.ToolOver);
		}
		while (bOK && json.comma());
		if (bOK)
			bOK = json.endObject();
	}
	return bOK;
}

static bool arrow(Theme& self, JSON::Reader &json)
{
	bool bOK = false;
	if ( json.beginNamedObject("Arrow") )
	{
		do
		{
			bOK = Font::load(json, self.Arrow) ||
				json.namedValue("up", self.ArrowUp) ||
				json.namedValue("dn", self.ArrowDn) ||
				json.namedValue("lt", self.ArrowLt) ||
				json.namedValue("rt", self.ArrowRt);
		}
		while (bOK && json.comma());
		if (bOK)
			bOK = json.endObject();
	}
	return bOK;
}

static bool loadTheme(Theme& self, JSON::Reader &json)
{
	bool bOK = json.beginNamedObject("Theme");
	if (bOK) do
	{
		bOK = Font::load(json, self.Text) ||
			json.namedValue("gridline", self.GridLines) ||
			tagForeBack(json, "Caption", self.CaptionFore, self.CaptionBack) ||
			tagForeBack(json, "Data", self.DataFore, self.DataBack) ||
			tagForeBack(json, "Cell", self.CellFore, self.CellBack) ||
			tagForeBack(json, "Row", self.RowFore, self.RowBack) ||
			tool(self, json) ||
			arrow(self, json);
	}
	while (bOK && json.comma());
	if (bOK)
		bOK = json.endObject();
	return bOK;
}

bool Theme::load(const TCHAR *path)
{
	FileInputStream stream;
	if ( stream.Open(path) )
	{
		JSON::Reader json;
		return json.open(&stream) && loadTheme(*this, json);
	}
	return false;
}

// retrieve theme color.
color_t Theme::getColor(const Color &color) const
{
	color_t rgb = color.color;
	switch(color.index)
	{
		case eCaptionFore:
			rgb = CaptionFore;
			break;
		case eCaptionBack:
			rgb = CaptionBack;
			break;

		case eDataFore:
			rgb = DataFore;
			break;
		case eDataBack:
			rgb = DataBack;
			break;

		case eCellFore:
			rgb = CellFore;
			break;
		case eCellBack:
			rgb = CellBack;
			break;

		case eRowFore:
			rgb = RowFore;
			break;
		case eRowBack:
			rgb = RowBack;
			break;

		case eGridLines:
			rgb = GridLines;
			break;

		case eToolFore:
			rgb = ToolFore;
			break;
		case eToolBack:
			rgb = ToolBack;
			break;
		case eToolOver:
			rgb = ToolOver;
			break;

		case eDefault:
		default:
			break;
	}
	return rgb;
}

// retrieve grid line thickness
meter_t Theme::getThick(const Thick &thick) const
{
	return thick.local ? thick.thick : GridThick;
}

// retrieve font
const Font& Theme::getFont(const Theme::Font &font) const
{
	switch (font.index)
	{
		case eText:
			return Text;

		case eArrow:
			return Arrow;

		case eStock:
			return Stock;

		case eDefault:
			break;
	}
	return font.font;
}

void Theme::setHeight(meter_t height)
{
	Text._height = height;
	Arrow._height = height;
	Stock._height = height;
}

void Theme::broadcast(const char *log) const
{
	Change.issueNotice(const_cast<Theme *>(this), log);
}

// retrieve glyph
const string_t& Theme::getGlyph(const Glyph &glyph) const
{
	return getGlyph(glyph.index, glyph.glyph);
}

// retrieve glyph
const string_t& Theme::getGlyph(unsigned char iGlyph, const string_t &other) const
{
	switch (iGlyph)
	{
		case eUp:
			return ArrowUp;

		case eDn:
			return ArrowDn;

		case eLt:
			return ArrowLt;

		case eRt:
			return ArrowRt;

		case eChecked:
			return Checked;

		case eUncheck:
			return Uncheck;

		case eExpand:
			return Expand;

		case eCollapse:
			return Collapse;

		case eDefault:
		default:
			break;
	}
	return other;
}
