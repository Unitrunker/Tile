#include "Flow.h"
#include "Table.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

namespace Tiles
{

struct Button;

struct Grid : public Flow, public INotify
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
};

};