#include "stdafx.h"
#include "History.h"
#include "Types.h"

// true if site matches.
bool Item::evalSite(unsigned short site) const
{
	return _site == site;
}

// true if site & LCN match.
bool Item::evalLCN(unsigned short site, unsigned short lcn) const
{
	return _site == site && _lcn == lcn;
}

// true if user matches.
bool Item::evalUser(unsigned long user) const
{
	switch (_verb)
	{
		case eCallsUser:
			return _source == user || _target == user;

		case eCallsGroup:
		case eJoins:
		case eLeaves:
		case eSignsOn:
		case eSignsOff:
			return _source == user;

		default:
			break;
	}
	return false;
}

// true if group matches.
bool Item::evalGroup(unsigned long group) const
{
	switch (_verb)
	{
		case eDrops:
		case eAdds:
			return _source == group || _target == group;

		case eCallsGroup:
		case eJoins:
		case eLeaves:
			return _target == group;

		default:
			break;
	}
	return false;
}

// true if service and class match.
bool Filter::eval(const Item &item) const
{
	return 
		item._stamp >= range[0] &&	// in time range?
		item._stamp < range[1] &&
		item._service < _countof(services) && 
		services[item._service] &&	// accepts service?
		item._verb < _countof(verbs) && 
		verbs[item._verb];			// accepts verb?
}

// filter user implementation.
bool FilterUser::eval(const Item &item) const
{
	return item.evalUser(_user) && Filter::eval(item);
}

// filter group implementation.
bool FilterGroup::eval(const Item &item) const
{
	return item.evalGroup(_group) && Filter::eval(item);
}

// filter site implementation.
bool FilterSite::eval(const Item &item) const
{
	return item.evalSite(_site) && Filter::eval(item);
}

// filter lcn implementation.
bool FilterLCN::eval(const Item &item) const
{
	return item.evalLCN(_site, _lcn) && Filter::eval(item);
}

bool Cursor::index(size_t &i) const
{
	if (forward)
	{
		i++;
		return true;
	}
	else if (i > 0)
	{
		i--;
		return true;
	}
	return false;
}

HistoryFlat::HistoryFlat()
{
	memset(&_item, 0, sizeof _item);
}

bool HistoryFlat::open()
{
	return _sam.open("History.dat", 0, sizeof _item);
}

bool HistoryFlat::close()
{
	return _sam.close();
}

// Retrieve up to "N" items where N is list.size().
// Items must match filter and lie adjacent to Cursor.
// Returns false on IO error.
bool HistoryFlat::get(const Filter &filter, Cursor &cursor, std::vector<Item> &list)
{
	// number of items found.
	size_t i = 0;
	// record index to start scan.
	size_t index = cursor.indices[cursor.forward];
	// update opposing scan index.
	cursor.indices[!cursor.forward] = index;

	// is our list full?
	while ( i < list.size() )
	{
		// no: read a record.
		if ( !read(index) )
			// IO error.
			return false;

		// match filter criteria?
		if ( filter.eval(_item) )
		{
			// yes: add it.
			list[i++] = _item;
		}

		// advance cursor; stop if reach limit.
		if ( !cursor.index(index) )
			break;
	}
	// resize to number records found.
	list.resize(i);
	// update cursor index.
	cursor.indices[cursor.forward] = index;
	return true;
}

// Binary search by time stamp.
// Slight IO penalty as the record is discarded - a future optimization.
bool HistoryFlat::seek(time_t stamp, Cursor &cursor)
{
	size_t index = 0;
	size_t lo = 0;
	size_t hi = _sam.size();

	// binary search.
	while (hi > lo)
	{
		index = (hi + lo) >> 1;
		if ( read(index) )
		{
			if (stamp > _item._stamp)
				lo = index + 1;
			else if (stamp < _item._stamp)
				hi = index;
			else 
				lo = hi = index;
		}
		else 
			return false; // IO failure.
	}

	// A binary search can land anywhere in a consecutive run of equal timestamps.
	// Based upon direction, seek to top or bottom of the run.
	// For forward, seek to lowest record index with matching timestamp.
	// For reverse, seek to highest record index with matching timestamp.

	// forward cursor
	if (cursor.forward)
	{
		size_t i = index;
		// current indexed record equals timestamp?
		while (i > 0 && _item._stamp == stamp)
		{
			// yes: save its index and try next one.
			index = i--;
			if (!read(i) )
				break;
		}
	}
	// reverse cursor.
	else
	{
		size_t i = index;
		// current indexed record equals timestamp?
		while (i < _sam.size() && _item._stamp == stamp)
		{
			// yes: save its index and try next one.
			index = i++;
			if (!read(i) )
				break;
		}
	}
	// set cursor indices
	cursor.indices[0] = cursor.indices[1] = index;
	return true;
}

// read indexed record.
bool HistoryFlat::read(size_t index)
{
	unsigned char *octets = 
		reinterpret_cast<unsigned char *>(&_item);
	return _sam.read(octets, index);
}

Verb::Verb(Tiles::SetT<Item> &set) : _set(set)
{
}

const Tiles::string_t &Verb::getValue() const
{
	static const TCHAR *list[] = 
	{
		_T("?"), _T("Call"), _T("Call"), _T("Add"), _T("Drop"), _T("Logon"), _T("Logoff"), _T("Join"), _T("Leave")
	};

	if (_set.size())
	{
		unsigned char value = _set.at(0)->_verb;
		if (value > 0 && value < _countof(list))
			_text = list[value];
	}
	else
		_text.clear();

	return _text;
}

bool Verb::setValue(const Tiles::string_t &value)
{
	value;
	return false;
}

HistorySet::HistorySet(Tiles::Theme& theme) :
	_site(NULL),
#pragma warning(disable:4355)
	_stamp(*this, &Item::_stamp),
	_source(*this, &Item::_source),
	_target(*this, &Item::_target),
	verb(*this),
#pragma warning(default:4355)
	Stamp(_stamp, Tiles::Time::eDate),
	Source(_source, 10),
	Target(_target, 10)
{
	theme;
}

static void instant()
{
	Tiles::Theme theme;
	Model::Folder folder;
	Model::network_t network = {Model::P25, 0x12345678};
	Model::System *system = folder.newSystem(network, true);
	Model::Site *site = system->newSite(1, true);
	FilterSite filter(1);
	Cursor cursor;
	HistoryFlat flat;
	HistorySet set(theme);
	HistoryTable table(theme);
	table.setContent(&flat, &filter, &cursor);
}
