#pragma once

#include "../Tile/Accessor.h"
#include "../Tile/Property.h"
#include "../Tile/Theme.h"
#include "../Tile/AVL.h"
#include "../Tile/Table.h"
#include "Model.h"

#include <vector>
#include <time.h>
#include "ISAM.h"

// list of verbs
enum {eCallsGroup, eCallsUser, eJoins, eLeaves, eAdds, eDrops, eSignsOn, eSignsOff};
// TODO: list of services.

// This log item records a specific network event.
// This is a POD representation of data that exists as fixed size records in a flat file table.
struct Item
{
	// time stamp of event.
	time_t _stamp;
	// action and type of service.
	unsigned char _verb, _service;
	// site and optional lcn where this happened.
	unsigned short _site, _lcn;
	// optional source and target addresses - usually group and user ids.
	unsigned long _source, _target;

	// evaluation functions used for filtering.
	bool evalLCN(unsigned short site, unsigned short lcn) const;
	bool evalSite(unsigned short site) const;
	bool evalUser(unsigned long user) const;
	bool evalGroup(unsigned long group) const;

	void broadcast(const char *) { };
};

// Search filter.
struct Filter
{
	// time range - lo and hi values.
	time_t range[2];

	// boolean vector of accepted verbs.
	// eg. verbs[0] = true, accepts verb zero.
	bool verbs[16];

	// boolean vector of accepted services.
	// eg. services[0] = true, accepts service zero.
	bool services[16];

	// Returns true if item matches this filter.
	// Extensible by derived class to add more criteria.
	virtual bool eval(const Item &) const;
};

// Four filters - user, group, site, and LCN.

struct FilterUser : public Filter
{
	FilterUser(unsigned short user) : _user(user) { };
	virtual bool eval(const Item &) const;
private:
	unsigned short _user;
};

struct FilterGroup : public Filter
{
	FilterGroup(unsigned short group) : _group(group) { };
	virtual bool eval(const Item &) const;
private:
	unsigned short _group;
};

struct FilterSite : public Filter
{
	FilterSite(unsigned short site) : _site(site) { };
	virtual bool eval(const Item &) const;
private:
	unsigned short _site;
};

struct FilterLCN : public Filter
{
	FilterLCN(unsigned short site, unsigned short lcn) : _site(site), _lcn(lcn) { };
	virtual bool eval(const Item &) const;
private:
	unsigned short _site;
	unsigned short _lcn;
};

// Tracks the upper and lower extents in the scan for records that match.
// We want both extents in case user reverses direction when scrolling 
// through paginated data.
struct Cursor
{
	// true to scan forward.
	bool forward;
	// upper and lower bound of scan.
	size_t indices[2];

	bool index(size_t &i) const;
};

// Flat data file as a table in chronological order.
// TODO: make this span multiple files.
// eg. each file covers a 24 hour period.
struct HistoryFlat
{
	HistoryFlat();

	// Open for IO.
	bool open();

	// Close IO.
	bool close();

	// Retrieve first "N" records that match filter. 
	// Start scan from cursor location.
	// Side effect: update cursor position.
	bool get(const Filter &, Cursor &, std::vector<Item> &list);

    // binary search by time stamp.
	// adjust timestamp to skip ahead or back an hour.
    bool seek(time_t stamp, Cursor &);
private:
	// read indexed record.
    bool read(size_t index);

	ISAM _sam;
	Item _item;
};

// accessor adapter for string to verb.
struct Verb : public Tiles::IAccessor<Tiles::string_t>
{
	Verb(Tiles::SetT<Item> &set);
	virtual const Tiles::string_t &getValue() const;
	virtual bool setValue(const Tiles::string_t &value);

private:
	Tiles::SetT<Item> &_set;
	mutable Tiles::string_t _text;
	Verb& operator = (const Verb &never);
};

struct HistorySet : public Tiles::SetT<Item>
{
	HistorySet(Tiles::Theme&);
private:
	Tiles::Reflect<Item, Model::address_t> _source;
	Tiles::Reflect<Item, Model::address_t> _target;
	Tiles::Reflect<Item, time_t> _stamp;

	Tiles::Time Stamp;
	Verb verb;
	Tiles::UInteger Source;
	Tiles::UInteger Target;
	Model::Site* _site;
};

// S presumed to derive from SetT<T>.
// Naive flat file implementation where each record is fixed byte size.
// Byte order is not portable!
// Must match type T's memory layout which can be compiler dependent (packing, alignment, optimization).
// A field stuffing strategy (eg. bencode) can be added later.
struct HistoryTable : public Tiles::ITable, public INotify
{
	Tiles::Theme& _theme;

	// content provider for a Grid.
	HistoryTable(Tiles::Theme &theme) : 
		_theme(theme), _notify(NULL), _bAscending(true), _readOnly(false), _flat(NULL), _filter(NULL), _cursor(NULL)
	{
		_header = new HistorySet(theme);
	}

	bool setContent(HistoryFlat *flat, Filter *filter, Cursor *cursor)
	{
		_flat = flat;
		_filter = filter;
		_cursor = cursor;
		return true;
	}

	// choose a sort column
	virtual bool setColumn(size_t iColumn)
	{
		iColumn;
		// ignore column - use the underlying file's sort order (eg. chonological).
		return false;
	}

	// disassociate from containers.
	virtual void clear()
	{
		size_t i = _visible.size();
		while (i)
		{
			delete _visible[--i];
		}
		_visible.clear();
	}

	// Specify scroll offset and number of visible rows.
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
			HistorySet *set = new HistorySet(_theme); // , _visible.size();
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

		if (_flat)
		{
			_content.resize(rows);
			_flat->get(*_filter, *_cursor, _content);
		}
		else
			_content.resize(0);

		size_t size = this->size();
		for (size_t i = 0; i < rows; i++)
		{
			size_t index = getAscendingIndex(i + _offset);
			if (index < size)
			{
				_visible[i]->setObject(&_content[i]);
			}
		}
	}

	// visible row index, not absolute row index.
	virtual Tiles::Set *getRow(size_t row)
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
	virtual Tiles::Set* getHeader()
	{
		return _header;
	}

	// returns total row count.
	virtual size_t size() const
	{
		return _content.size();
	}

	virtual void follow(INotify *notify)
	{
		_notify = notify;
	}

	// Enable the table for editing. Store setting in the header and also set all visible rows.
	// History is read only so this doesn't do much.
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
		index;
		return false;
	}

	// multi-select
	virtual void setSelect(size_t r1, size_t r2)
	{
		r1;
		r2;
	}

	// single-select
	virtual void setSelect(size_t index, bool select)
	{
		index;
		select;
	}

	// clear all selected rows.
	virtual void clearSelect()
	{
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
	// These will never happen.
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
	HistorySet* _header;
	// list of sets for visible rows.
	// each set binds to a different row of data.
	std::vector<HistorySet*> _visible;
	// list of data providing content for above sets.
	std::vector<Item> _content;

	HistoryFlat *_flat;
	Cursor* _cursor;
	Filter* _filter;
	bool _bAscending;
	bool _readOnly;
	INotify *_notify;
};
