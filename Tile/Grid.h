#include "Pane.h"
#include "Table.h"
#include "Scroll.h"
#include "Delegate.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Button;

// Grid control
// TODO: multi-row selection
struct Grid : public Pane, public INotify
{
	// Double click delegate - notifies when cell is double clicked.
	sophia::delegate3<void, Grid *, size_t, size_t> DoubleClick;
	// Create a grid control.
	Grid(identity_t id, Theme &theme);
	// Sets the content for this grid using an interface that is oblivious to the underlying data type.
	void setTable(ITable *p, bool multi = false);
	// Set bounding rectangle.
	virtual void setRect(const rect_t &rect);
	// Clear selection.
	void clearSelect();

	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pControl);

protected:
	virtual void onIndexChanged(size_t index);
private:
	void clickHeader(Button *control, bool value);

	size_t getRowIndex() const;

	void onRowIndexChanged();

    size_t getVisibleRowCount();
	// called when grid resized or new table assigned.
	void reflow();

	// IControl
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);

	// row "i" added.
	virtual void onAdded(size_t i);
	// row "i" changed.
	virtual void onChange(size_t i);
	// row "i" removed.
	virtual void onRemove(size_t i);
	// row "i" moved to row "j".
	virtual void onMoved(size_t i, size_t j);

	// These next three fields used for column width resizing.
	// Reference for dragging column divider to adjust width.
	meter_t _datum;
	// True for mouse capture while drag operation is active.
	bool _capture;
	// The column whose width is being adjusted.
	size_t _drag;
	// most recent row index
	size_t _cursor;

	bool _multi;

	// The grid's content.
	ITable* _table;
	// TODO: scrolling
	// Plan is to embed a vertical flow in the grid.
	// The vertical flow will contain the data rows.
	// An additional row at the bottom will be the horizontal scroll.
	// The Grid flow will itself be made to a horizontal flow.
	// Its two immediate children will be the vertical flow and a vertical scroll.
	// The two scroll controls will be hidden as needed by setting their width to zero.
	// TODO: there is probably need for a scroll enabled flow.
	// The scrollbar flows will be {1, 1, 0, true}

	// TODO: vertical scrolling.
	Scroll* _scrollVert;
	Pane* _paneVert;

	// TODO: horizontal scrolling.
	Scroll* _scrollHorz;
};

};