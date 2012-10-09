#include "stdafx.h"
#include "Table.h"

// I forget what this does. 8^P
void Table::clear()
{
}

// starting row for page and number of rows to display.
void Table::setVisible(size_t offset, size_t count)
{
	if (count != _rows.size())
	{
		_offset = -1;
		_rows.resize(count);
	}
	setOffset(offset);
}

// return scrolling offset into table.
size_t Table::getOffset() const;
{
	return _offset;
}

// page level navigation.
void Table::setOffset(size_t offset)
{
	// TODO!!
}

// return visible row's property set.
Set *Table::getRow(size_t row)
{
	if (row < _offset)
		return NULL;
	row -= _offset;
	if (row < _rows.size())
		return _rows[row];
	return NULL;
}

// return header's property set.
Set* Table::getHeader()
{
	return _header;
}

