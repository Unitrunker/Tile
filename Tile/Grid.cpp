#include "stdafx.h"
#include "Grid.h"
#include "Text.h"
#include "Fill.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Grid::Grid(identity_t id, Theme &theme) : Flow(id, theme, eDown), _table(NULL)
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
	// compute number of visible rows
	size_t rows = getVisibleRowCount();

	// controls are tiles to deleting all tiles also deletes all controls.
	_listControls.clear();
	// delete all tiles.
	size_t i = _listTiles.size();
	while (i)
	{
		ITile *pKill = _listTiles[--i];
		delete pKill;
	}
	_listTiles.clear();

	if (_table)
	{
		_table->setVisible(0, rows);

		Set *set = _table->getHeader();
		Flow *row = new Flow(0, theme, eRight);
		for (size_t col = 0; col < set->Columns.size(); col++)
		{
			Text *text = new Text(0, theme, textFont, eLeft, set->Columns[col]->Name);
			row->Add(text);
		}
		Add(row);

#if 0
		Fill *pFill = new Fill(0, theme);
		Add(pFill);
#else
		for (i = 0; i < rows; i++)
		{
			set = _table->getRow(i);
			row = new Flow(0, theme, eRight);
			if (set == NULL)
			{
				Fill *pFill = new Fill(0, theme);
				row->Add(pFill, 0, 4096, 0);
			}
			else
			{
				for (size_t col = 0; col < set->Columns.size(); col++)
				{
					IControl *pControl = set->Columns[col]->Control;
					row->Add(pControl);
				}
			}
			Add(row);
		}
#endif
	}
	else
	{
		Fill *pFill = new Fill(0, theme);
		Add(pFill);
	}
	Flow::reflow();
}

void Grid::setRect(const rect_t &rect)
{
	Flow::setRect(rect);
	reflow();
}

size_t Grid::getVisibleRowCount()
{
	const Theme& theme = _tile.getTheme();
	meter_t height = theme.Text._height;
	rect_t rect = {0};
	_tile.getRect(rect);

	meter_t space = theme.getThick(_space);

	// compute number of visible rows
	size_t rows = (rect.high + space) / (height + space);

	if (rows > 1)
		rows--;

	return rows;
}