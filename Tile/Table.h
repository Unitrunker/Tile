#include "Property.h"
#include "AVL.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct IRowNotify
{
   // row "i" added.
   virtual void onAdded(size_t i) = 0;
   // row "i" changed.
   virtual void onChange(size_t i) = 0;
   // row "i" removed.
   virtual void onRemove(size_t i) = 0;
   // row "i" moved to row "j".
   virtual void onMoved(size_t i, size_t j) = 0;
};

// table interface for Grid control.
struct ITable
{
	virtual void clear() = 0;
	virtual void setVisible(size_t offset, size_t count) = 0;
	virtual size_t getOffset() const = 0;
	virtual void setOffset(size_t offset) = 0;
	virtual Set *getRow(size_t row) = 0;
	virtual Set* getHeader() = 0;
	virtual void follow(IRowNotify *) = 0;
	virtual size_t getCount() const = 0;
};

// S presumed to derive from SetT<T>.
template <typename S, typename T, typename tree_t>
struct Table : public ITable, public INotify<T*>
{
	Theme& _theme;

	Table(Theme &theme) : _theme(theme), _tree(NULL), _notify(NULL)
	{
		_header = new S(_theme);
	}

	void setContent(tree_t *tree)
	{
		if (_tree)
			_tree->ignore(this);
		_tree = tree;
		_tree->follow(this);
	}

	virtual void clear()
	{
		if (_tree)
			_tree->ignore(this);

		_tree = NULL;
		size_t i = _visible.size();
		while (i)
		{
			delete _visible[--i];
		}
		_visible.clear();
	}

	bool add(T *value)
	{
		return _tree != NULL && _tree->insert(value);
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

		for (size_t i = 0; i < rows; i++)
		{
			size_t index = i + _offset;
			if (index < size())
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

	size_t size() const
	{
		return _tree != NULL ? _tree->size() : 0;
	}

	void follow(IRowNotify *notify)
	{
		_notify = notify;
	}

	virtual size_t getCount() const
	{
		return size();
	}

private:
	// Node "t" at index "i" added.
	virtual void onAdded(size_t i, T*)
	{
		if (_notify)
			_notify->onAdded(i);
	}
	// Node "t" at index "i" changed.
	virtual void onChange(size_t i, T*)
	{
		if (_notify)
			_notify->onChange(i);
	}
	// Node "t" at index "i" removed.
	virtual void onRemove(size_t i, T*)
	{
		if (_notify)
			_notify->onRemove(i);
	}
	// Node "t" at index "i" moved to index "j".
	virtual void onMoved(size_t i, size_t j, T*)
	{
		if (_notify)
			_notify->onMoved(i, j);
	}

	size_t _offset;
	S* _header;
	std::vector<S *> _visible;
	tree_t* _tree;
	IRowNotify *_notify;
};

}