#include "Flow.h"
#include "Property.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

struct Grid : public Flow
{
	Grid(identity_t id, Theme &theme);
	void setTable(ITable *p);
	virtual void setRect(const rect_t &rect);

private:
    size_t getVisibleRowCount();
	// called when grid resized or new table assigned.
	void reflow();

	ITable* _table;
};

};