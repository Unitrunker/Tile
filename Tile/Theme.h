#include "Font.h"
#include "Delegate.h"
#include "Multicast.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Theme
{
	// shared index
	enum
	{
		eDefault = 0
	};

	// color indices
	enum
	{
		eCaptionFore = 1,
		eCaptionBack,
		eDataFore,
		eDataBack,
		eCellFore,
		eCellBack,
		eRowFore,
		eRowBack,
		eGridLines,
		eToolFore,
		eToolBack,
		eToolOver
	};

	// font indices
	enum
	{
		eText = 1,
		eArrow,
		eStock
	};

	// glyph indices
	enum
	{
		eUp = 1,
		eDn,
		eLt,
		eRt,
		eChecked,
		eUncheck,
		eExpand,
		eCollapse
	};

	// color selector
	struct Color
	{
		unsigned char index;
		color_t color;
	};

	// thickness selector
	struct Thick
	{
		bool local;
		meter_t thick;
	};

	// font selector
	struct Font
	{
		unsigned char index;
		Tiles::Font font;
	};

	// font selector
	struct Glyph
	{
		unsigned char index;
		string_t glyph;
	};

	// Change delegate - notifies when theme changes.
	Multicast< sophia::delegate1<void, const Theme*>, Theme > Change;

	// Caption
	color_t CaptionFore;
	color_t CaptionBack;
	// Data
	color_t DataFore;
	color_t DataBack;
	// active cell in a grid
	color_t CellFore;
	color_t CellBack;
	// active row in a grid
	color_t RowFore;
	color_t RowBack;
	// for grids and outlines
	color_t GridLines;
	meter_t GridThick;
	// Toolbar glyphs
	color_t ToolFore;
	color_t ToolBack;
	color_t ToolOver;
	// application font
	Tiles::Font Text;
	// symbol font for arrows
	Tiles::Font Arrow;
	// specific glyphs for each of the four arrows.
	string_t ArrowUp;
	string_t ArrowDn;
	string_t ArrowLt;
	string_t ArrowRt;

	// checkbox glyphs.
	string_t Checked;
	string_t Uncheck;

	// List control glyphs.
	string_t Expand;
	string_t Collapse;

	Tiles::Font Stock;

	Theme();
	Theme(const Theme &copy);

	bool save(const TCHAR *path);
	bool load(const TCHAR *path);

	void broadcast(const char *log) const;

	// retrieve theme color.
	color_t getColor(const Color &color) const;
	// retrieve grid line thickness
	meter_t getThick(const Thick &thick) const;
	// retrieve font
	const Tiles::Font& getFont(const Font &font) const;
	void setHeight(meter_t height);
	// retrieve glyph
	const string_t& getGlyph(const Glyph &glyph) const;
	// get name of the theme color index.
	static const TCHAR *getName(char index);
};

};