#include "Theme.h"
#include "Accessor.h"
#include "IControl.h"
#include <vector>
#include <map>

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

// A property - used to populate one key/value entry in a property List or one cell in a Grid.
struct Property
{
	const TCHAR *Name;
	const TCHAR *Notes;
	IControl *Control;

	Property(const TCHAR *name, const TCHAR *notes, IControl *);
	~Property();
};

// A property section.
struct Section
{
	const TCHAR *Caption;
	const TCHAR *Notes;
	std::vector<Property *> Items;

	Section(const TCHAR *, const TCHAR *);
	~Section();
	void Add(Property *);
};

// A property set container.
// This base class is recognized by the List and Grid controls.
// This is the portion of your custom property set seen by these controls.
struct Set
{
	std::vector<Section*> Sections;
	std::vector<Property*> Columns;
	void Add(Section *);
	virtual ~Set();
};

// A type specialized property set base class - the Value accessor allows swapping out object pointers.
// For a business object of type 'X', derive your custom property set from base class SetT<X>.
template <typename T>
struct SetT : public Value<T *>, public Set
{
	SetT(T *p) : Value(p) { };
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
};

// S presumed to derive from SetT<T>.
template <typename S, typename T>
struct Table : public ITable
{
	Theme& _theme;

	Table(Theme &theme) : _theme(theme)
	{
		_header = new S(_theme);
	}

	virtual void clear()
	{
		_map.clear();
		size_t i = _visible.size();
		while (i)
		{
			delete _visible[--i];
		}
		_visible.clear();
	}

	void add(std::vector<unsigned char> &key, T *value)
	{
		_map[key] = value;
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

		std::map< std::vector<unsigned char>, T *>::iterator it = _map.begin();
		// TODO: replace std::map with own balanced binary tree that supports indexed random access.
		if (_map.size() > offset)
		{
			// horrific
			while (offset)
			{
				it ++;
				offset--;
			}
		}
		else
			it = _map.end();
		for (size_t i = 0; i < _visible.size(); i++)
		{
			if (it != _map.end())
			{
				_visible[i]->setValue(it->second);
				it++;
			}
		}
	}

	// visible row index, not absolute row index.
	virtual Set *getRow(size_t row)
	{
		if (row < _visible.size())
		{
			size_t loc = row + _offset;
			if (loc < _map.size())
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

private:
	size_t _offset;
	S* _header;
	std::vector<S *> _visible;
	std::map< std::vector<unsigned char>, T *> _map;
};

};