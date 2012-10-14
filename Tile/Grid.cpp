#include "stdafx.h"
#include "Grid.h"
#include "Text.h"
#include "Fill.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

struct Row : public Flow
{
	Row(identity_t id, Theme &theme);
	Row(identity_t id, Theme &theme, Theme::Font& desc);
	virtual ~Row() { };
};

Row::Row(identity_t id, Theme &theme) : Flow(id, theme, eRight)
{
}

Row::Row(identity_t id, Theme &theme, Theme::Font& desc) : Flow(id, theme, desc, eRight)
{
}

struct Header : public Flow
{
	Header(identity_t id, Theme &theme);
	Header(identity_t id, Theme &theme, Theme::Font& desc);
	virtual ~Header() { };
};

Header::Header(identity_t id, Theme &theme) : Flow(id, theme, eRight)
{
}

Header::Header(identity_t id, Theme &theme, Theme::Font& desc) : Flow(id, theme, desc, eRight)
{
}

Grid::Grid(identity_t id, Theme &theme) : Flow(id, theme, eDown), _table(NULL)
{
}

// Sets the content for this grid.
void Grid::setTable(ITable *p)
{
	_table = p;
	reflow();
}

// Called when grid resized or new table assigned.
// This implementation has plenty of room for improvement.
// Currently everything is destroyed and recreated.
// TODO: rewrite to only create / delete rows as needed.
void Grid::reflow()
{
	Theme& theme = _tile.getTheme();
	Theme::Font textFont = {Theme::eText, theme.Text};
	// compute number of visible rows
	size_t rows = getVisibleRowCount();

	// controls are tiles so deleting all tiles also deletes all controls.
	_listControls.clear();
	// delete all tiles.
	size_t i = _listTiles.size();

	// all but first row are data rows.
	while (i > 1)
	{
		// get next data row.
		Row *pKill = reinterpret_cast<Row *>(_listTiles[--i]);
		// remove cell tiles before deleting the row 'cuz the cell tiles belong to the table.
		pKill->clear();
		delete pKill;
	}

	// first row is header row.
	if (i > 0)
	{
		ITile *pKill = _listTiles[--i];
		// delete the header row (all of which is owned by this grid).
		delete pKill;
	}

	_listTiles.clear();

	if (_table)
	{
		_table->setVisible(0, rows);

		// compose a header row.
		Set *set = _table->getHeader();
		Header *row = new Header(0, theme);
		for (size_t col = 0; col < set->Columns.size(); col++)
		{
			// TODO: include a "splitter" control with each Text 
			// tile to allow users to adjust column widths.
			FlowDesc desc = {0};
			// Text will be our column heading.
			// TODO: replace Text with text Button. Button click for column sorting.
			Text *text = new Text(0, theme, textFont, eLeft, set->Columns[col]->Name);
			// Cloning the controls flowdesc causes the header tiles 
			// to align proportionately with the data cells.
			IControl *pControl = set->Columns[col]->Control;
			pControl->getFlow(eRight, desc);
			text->setFlow(eRight, desc);
			// add column heading to header row.
			row->Add(text);
		}
		// add the header row.
		Add(row);

		for (i = 0; i < rows; i++)
		{
			// compose data rows.
			set = _table->getRow(i);
			Row *row = new Row(0, theme);
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
			// add data row.
			Add(row);
		}
	}
	else
	{
		// no table so use empty filler.
		Fill *pFill = new Fill(0, theme);
		Add(pFill);
	}
	Flow::reflow();
}

void Grid::setRect(const rect_t &rect)
{
	_tile.setRect(rect);
	reflow();
}

size_t Grid::getVisibleRowCount()
{
	const Theme& theme = _tile.getTheme();
	meter_t height = theme.Text._height;
	rect_t rect = {0};
	_tile.getRect(rect);

	meter_t space = theme.getThick(_space);

	// compute number of visible rows.
	size_t rows = (rect.high + space) / (height + space);

	if (rows > 1)
		rows--;

	return rows;
}
