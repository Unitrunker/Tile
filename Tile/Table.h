#include "Property.h"
#include "AVL.h"
#include <map>

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

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
};

// S presumed to derive from SetT<T>.
template <typename S, typename T>
struct Table : public ITable, public INotify
{
	typedef IRowSet<T*> tree_t;
	typedef std::map<size_t, tree_t *> columns_t;
	Theme& _theme;

	Table(Theme &theme) : _theme(theme), _tree(NULL), _notify(NULL), _bAscending(true), _iColumn(0)
	{
		_header = new S(_theme);
	}

	// typeaware portion, need not be virtual.
	// specifies which ordered tree to associate with which column.
	// The same tree can be used for multiple columns.
	void setContent(size_t iColumn, tree_t *tree)
	{
		bool bFirst = _trees.size() == 0;
		_trees[iColumn] = tree;
		if (bFirst)
		{
			_tree = tree;
			_iColumn = iColumn;
			tree->follow(this);
		}
	}

	// choose a sort column
	virtual bool setColumn(size_t iColumn)
	{
		if (iColumn == _iColumn)
		{
			_bAscending = !_bAscending;
			setOffset(_offset);
		}
		else
		{
			columns_t::iterator it = _trees.find(iColumn);
			if (it != _trees.end())
			{
				// if two columns use the same ordering, 
				// act as if the column has not changed.
				// Toggle ascending / descending instead.
				if (_tree == _trees[_iColumn])
				{
					_bAscending = !_bAscending;
					setOffset(_offset);
				}
				else
				{
					_tree->ignore(this);
					_iColumn = iColumn;
					_tree = _trees[_iColumn];
					_tree->follow(this);
					setOffset(_offset);
				}
				return true;
			}
		}
		return false;
	}

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

	bool add(T* value)
	{
		columns_t::iterator it = _trees.begin();
		while (it != _trees.end())
		{
			tree_t *tree = *it++;
			tree->insert(value);
		}
		return true;
	}

	virtual void setVisible(size_t offset, size_t count)
	{
		if (_visible.size() > count)
		{
			size_t i = _visible.size();
			while (i > count)
			{
				delete _visible[--i];
			}
			_visible.resize(count);
		}
		else while (_visible.size() < count)
		{
			_visible.push_back(new S(_theme));
		}

		setOffset(offset);
	}

	virtual size_t getOffset() const
	{
		return _offset;
	}

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

	virtual Set* getHeader()
	{
		return _header;
	}

	virtual size_t size() const
	{
		return _tree != NULL ? _tree->size() : 0;
	}

	virtual void follow(INotify *notify)
	{
		_notify = notify;
	}

private:
	// Node "t" at index "i" added.
	virtual void onAdded(size_t i)
	{
		if (_notify)
			_notify->onAdded(i);
	}
	// Node "t" at index "i" changed.
	virtual void onChange(size_t i)
	{
		if (_notify)
			_notify->onChange(i);
	}
	// Node "t" at index "i" removed.
	virtual void onRemove(size_t i)
	{
		if (_notify)
			_notify->onRemove(i);
	}
	// Node "t" at index "i" moved to index "j".
	virtual void onMoved(size_t i, size_t j)
	{
		if (_notify)
			_notify->onMoved(i, j);
	}

	size_t _offset;
	S* _header;
	std::vector<S *> _visible;
	columns_t _trees;
	tree_t *_tree; // current tree.
	bool _bAscending;
	size_t _iColumn; // current column.
	INotify *_notify;
};

}