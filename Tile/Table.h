#include "Property.h"
#include "AVL.h"
#include <vector>
#include "Check.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

// BIG TODO: create implementation that uses metakit View as the container.
// The container can implement a mechanism where offline content that isn't
// cached is fetched asychronously. 
// A fake row appears in the grid while waiting for content to appear.
// When real data arrives, fake row is replaced by real one.

// Table interface for Grid control.
// Note this is NOT a template. No type awareness needed.
struct ITable
{
	virtual void clear() = 0;
	virtual void setVisible(size_t offset, size_t count) = 0;
	virtual size_t getOffset() const = 0;
	virtual void setOffset(size_t offset) = 0;
	virtual Set *getRow(size_t row) = 0;
	virtual Set* getHeader() = 0;
	// INotify comes from AVL.h
	virtual void follow(INotify *) = 0;
	virtual size_t size() const = 0;
	virtual bool setColumn(size_t) = 0;
	virtual void setEnable(bool bSet) = 0;
	virtual bool getEnable() const = 0;
	// select
	virtual bool getSelect(size_t index) const = 0;
	// multi-row select.
	virtual void setSelect(size_t r1, size_t r2) = 0;
	// single-select
	virtual void setSelect(size_t index, bool select) = 0;
	// clear all selected rows.
	virtual void clearSelect() = 0;
	// index adjusted for sort order.
	virtual size_t getAscendingIndex(size_t index) const = 0;
};

// S presumed to derive from SetT<T>.
// Caution: content is assumed to be IRowSet<T*> (eg. pointer to T).
template <typename S, typename T>
struct Table : public ITable, public INotify
{
	typedef IRowSet<T*> tree_t;
	typedef std::vector<tree_t *> columns_t;
	Theme& _theme;

	struct Selector : public IAccessor<bool>
	{
		Table<S, T> &_table;
		size_t _offset;
		mutable bool _result;

		Selector(Table<S, T> &table, size_t offset) : _table(table), _offset(offset) { }

		virtual const bool& getValue() const
		{
			size_t index = _table.getOffset() + _offset;
			index = _table.getAscendingIndex(index);
			_result = _table._tree->getSelect(index);
			return _result;
		}

		virtual bool setValue(const bool &value)
		{
			size_t index = _table.getOffset() + _offset;
			index = _table.getAscendingIndex(index);
			_table._tree->setSelect(index, value);
			return true;
		}
	};

	struct CheckGrid : public Tiles::Check
	{
		CheckGrid(identity_t id, Theme &theme, IAccessor<bool> *access, align_t align = eLeft) :
			Check(id, theme, access, align)
		{
		}

		CheckGrid(identity_t id, Theme &theme, bool &value, align_t align = eLeft) : 
			Check(id, theme, value, align)
		{
		}

		virtual bool getEnable() const
		{
			// ignore container's enable state.
			// we always want to click on row select checkbox.
			return _enable;
		}
	};

	// enhanced version of S that injects a checkbox for item selection.
	struct Splus : public S
	{
		Splus(Table<S, T> &table, size_t offset) : S(table._theme), _selector(table, offset)
		{
			// inject checkbox ahead of other columns.
			Tiles::Check *check = new CheckGrid(0, table._theme, &_selector);
			Tiles::Property *prop = new Tiles::Property(_T("@"), _T("Select this row"), check, true, false);
			Columns.insert(Columns.begin(), prop);
		}
	private:
		// checkbox wiring.
		Selector _selector;
	};

	// content provider for a Grid.
	Table(Theme &theme) : 
		_theme(theme), _tree(NULL), _notify(NULL), _bAscending(true), _iColumn(0), _readOnly(false)
	{
		// enhanced version of S that injects a checkbox for item selection.
		_header = new Splus(*this, 0);
		_trees.resize(_header->Columns.size(), NULL);
	}

	// typeaware portion, need not be virtual.
	// specifies which ordered tree to associate with which column.
	// The same tree can be used for multiple columns.
	void setContent(size_t iColumn, tree_t *tree)
	{
		// sane column?
		if (iColumn < _trees.size())
		{
			// yes: associate ordered content with this column.
			_trees[iColumn] = tree;
			// have column selection?
			if (!_tree)
			{
				// no: use this one as a default.
				_tree = tree;
				_iColumn = iColumn;
				tree->follow(this);
			}
		}
	}

	// choose a sort column
	virtual bool setColumn(size_t iColumn)
	{
		// sane column number.
		if (iColumn < _trees.size())
		{
			// get column?
			tree_t *tree = _trees[iColumn];
			// Do the two columns use the same ordering, 
			if (_tree == tree)
			{
				// Yes: act as if the column has not changed.
				// Toggle ascending / descending instead.
				_bAscending = !_bAscending;
				setOffset(_offset);
			}
			// new column exists?
			else if (tree != NULL)
			{
				// Yes
				// swap ordered containers.
				_tree->ignore(this);
				_iColumn = iColumn;
				_tree = tree;
				_tree->follow(this);
				setOffset(_offset);
			}
			return true;
		}
		return false;
	}

	// disassociate from containers.
	virtual void clear()
	{
		_tree->ignore(this);
		_tree = NULL;
		_trees.clear();
		size_t i = _visible.size();
		while (i)
		{
			delete _visible[--i];
		}
		_visible.clear();
	}

	// add value to all containers.
	bool add(T* value)
	{
		columns_t uniq;

		// Containers may be used for more than one column. 
		// Must avoid duplicates in the same container.
		// Construct a unique list of tree containers.
		columns_t::iterator it = _trees.begin();
		while (it != _trees.end())
		{
			bool bInsert = true;
			tree_t *tree = *it++;
			columns_t::iterator itu = uniq.begin();
			while (bInsert && itu != uniq.end())
			{
				if (tree == *itu++)
				{
					bInsert = false;
				}
			}
			if (bInsert)
				uniq.push_back(tree);
		}
		// Now have uniq list of containers.
		it = uniq.begin();
		// Add the value to each container - once!
		while (it != uniq.end())
		{
			tree_t *tree = *it++;
			tree->insert(value);
		}
		return true;
	}

	// specify scroll offset and number of visible rows.
	// forms a sliding window into the row set.
	virtual void setVisible(size_t offset, size_t count)
	{
		size_t vis = _visible.size();
		// excess visible rows?
		if (vis > count)
		{
			// delete surplus rows.
			while (vis > count)
			{
				delete _visible[--vis];
			}
			_visible.resize(count);
		}
		// shortage of rows?
		else while (_visible.size() < count)
		{
			// enhanced version of S that injects a checkbox for item selection.
			Splus *set = new Splus(*this, _visible.size());
			set->setEnable(getEnable());
			// yes: add them here.
			_visible.push_back(set);
		}
		// bind sets to data.
		setOffset(offset);
	}

	// get scrolling offset.
	virtual size_t getOffset() const
	{
		return _offset;
	}

	// set scrolling offset. 
	virtual void setOffset(size_t offset)
	{
		_offset = offset;

		size_t rows = _visible.size();

		size_t size = _tree->size();
		for (size_t i = 0; i < rows; i++)
		{
			size_t index = getAscendingIndex(i + _offset);
			if (index < size)
			{
				tree_t &tree = *_tree;
				_visible[i]->setObject(tree[index]);
			}
		}
	}

	// visible row index, not absolute row index.
	virtual Set *getRow(size_t row)
	{
		if (row < _visible.size())
		{
			size_t loc = row + _offset;
			if (loc < size())
			{
				return _visible[row];
			}
		}
		return NULL;
	}

	// retrieve property set used to describe header.
	virtual Set* getHeader()
	{
		return _header;
	}

	// returns total row count.
	virtual size_t size() const
	{
		return _tree != NULL ? _tree->size() : 0;
	}

	virtual void follow(INotify *notify)
	{
		_notify = notify;
	}

	// Enable the table for editing. Store setting in the header and also set all visible rows.
	virtual void setEnable(bool bSet)
	{
		if (_header)
			_header->setEnable(bSet);
		for (size_t i = 0; i < _visible.size(); i++)
			_visible[i]->setEnable(bSet);
	}

	// Default to true if somehow we have no header.
	virtual bool getEnable() const
	{
		return _header ? _header->getEnable() : true;
	}

	// select
	virtual bool getSelect(size_t index) const
	{
		return _tree->getSelect(index);
	}

	// multi-select
	virtual void setSelect(size_t r1, size_t r2)
	{
		if (r1 > r2)
		{
			size_t swap = r1;
			r1 = r2;
			r2 = swap;
		}
		while (r1 <= r2)
		{
			_tree->setSelect(r1, true);
			r1++;
		}
	}

	// single-select
	virtual void setSelect(size_t index, bool select)
	{
		_tree->setSelect(index, select);
	}

	// clear all selected rows.
	virtual void clearSelect()
	{
		_tree->clearSelect();
	}

	virtual size_t getAscendingIndex(size_t index) const
	{
		if (!_bAscending)
			index = size() - index - 1;
		return index;
	}

private:
	// row at "i" added.
	virtual void onAdded(size_t i)
	{
		if (_notify)
			_notify->onAdded(i);
	}
	// row at index "i" changed.
	virtual void onChange(size_t i)
	{
		if (_notify)
			_notify->onChange(i);
	}
	// row at index "i" removed.
	virtual void onRemove(size_t i)
	{
		if (_notify)
			_notify->onRemove(i);
	}
	// row index "i" moved to index "j".
	virtual void onMoved(size_t i, size_t j)
	{
		if (_notify)
			_notify->onMoved(i, j);
	}

	// scroll offset.
	size_t _offset;
	// header description
	S* _header;
	// list of sets for visible rows.
	// each set binds to a different row of data.
	std::vector<S *> _visible;
	// map of containers.
	columns_t _trees;
	tree_t *_tree; // current tree.
	bool _bAscending;
	bool _readOnly;
	size_t _iColumn; // current column.
	INotify *_notify;
};

}