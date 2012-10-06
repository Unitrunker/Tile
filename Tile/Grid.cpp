#include "stdafx.h"
#include "Grid.h"
#include "Text.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Grid::Grid(identity_t id, Theme &theme) : Flow(id, theme, eDown)
{
}

// the content for this grid.
void Grid::setTable(ITable *p)
{
	_table = p;
	reflow();
}

// called when grid resized or new table assigned.
void Grid::reflow()
{
	Theme& theme = _tile.getTheme();
	Theme::Font textFont = {Theme::eText, theme.Text};
	meter_t height = theme.Text._height;
	rect_t rect = {0};
	_tile.getRect(rect);

	meter_t space = theme.getThick(_space);

	// compute number of visible rows
	size_t rows = (rect.high + space) / (height + space);

	if (rows > 1)
		rows--;
	_table->setVisible(0, rows);

	Set *set = _table->getHeader();
	Flow *row = new Flow(0, theme, eRight);
	for (size_t col = 0; col < set->Columns.size(); col++)
	{
		Text *text = new Text(0, theme, textFont, eLeft, set->Columns[col]->Name);
		row->Add(text);
	}
	Add(row);

	for (size_t i = 0; i < rows; i++)
	{
		row = new Flow(0, theme, eRight);
		set = _table->getRow(i);
		for (size_t col = 0; col < set->Columns.size(); col++)
		{
			row->Add(set->Columns[col]->Control);
		}
		Add(row);
	}

	Flow::reflow();
}
