#include "Pane.h"
#include "Table.h"
#include "Scroll.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

namespace Tiles
{

struct Button;

// Grid control
struct Grid : public Pane, public INotify
{
	Grid(identity_t id, Theme &theme);
	void setTable(ITable *p);
	virtual void setRect(const rect_t &rect);

private:
	void clickHeader(Button *control, bool value);

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

	ITable* _table;
	// TODO: scrolling
	// Plan is to embed a vertical flow in the grid.
	// The vertical flow will contain the data rows.
	// An additional row at the bottom will be the horizontal scroll.
	// The Grid flow will itself be made to a horizontal flow.
	// It's two immediate children will be the vertical flow and a vertical scroll.
	// The two scroll controls will be hidden as needed by setting their width to zero.
	// TODO: there is probably need for a scroll enabled flow.

	// TODO: vertical scrolling.
	Scroll* _scrollVert;
	Pane* _paneVert;

	// TODO: horizontal scrolling.
	Scroll* _scrollHorz;
};

};