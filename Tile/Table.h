#include "Property.h"
#include "AVL.h"
#include <vector>

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
	virtual void setReadOnly(bool bSet) = 0;
	virtual bool getReadOnly() const = 0;
};

// S presumed to derive from SetT<T>.
// Cautioin: content is assumed to be IRowSet<T*> (eg. pointer to T).
template <typename S, typename T>
struct Table : public ITable, public INotify
{
	typedef IRowSet<T*> tree_t;
	typedef std::vector<tree_t *> columns_t;
	Theme& _theme;

	// content provider for a Grid.
	Table(Theme &theme) : 
		_theme(theme), _tree(NULL), _notify(NULL), _bAscending(true), _iColumn(0), _readOnly(false)
	{
		_header = new S(_theme);
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
			S *set = new S(_theme);
			set->setReadOnly(getReadOnly());
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
			size_t index = i + _offset;
			if (!_bAscending)
				index = size - index - 1;
			if (index < size)
			{
				tree_t &tree = *_tree;
				_visible[i]->setValue(tree[index]);
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

	// Make the table read-only. Store setting in the header and also set all visible rows.
	virtual void setReadOnly(bool bSet)
	{
		if (_header)
			_header->setReadOnly(bSet);
		for (size_t i = 0; i < _visible.size(); i++)
			_visible[i]->setReadOnly(bSet);
	}

	// Default to true if somehow we have no header.
	virtual bool getReadOnly() const
	{
		return _header ? _header->getReadOnly() : true;
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