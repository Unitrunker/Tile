#include "Theme.h"
#include "Accessor.h"
#include "IControl.h"
#include "Follow.h"
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
	const TCHAR *Name;		// Short name
	const TCHAR *Notes;		// Description
	IControl *Control;		// control that edits/displays property value.
	bool list;				// true to include in List control.
	bool grid;				// true to include in Grid control.


	Property(const TCHAR *name, const TCHAR *notes, IControl *, bool grid = true, bool list = true);
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
	Set(bool enable = true);
	virtual ~Set();
	void Add(Section *);
	void setEnable(bool bSet);
	bool getEnable() const;
	void onChange();
	void apply();
private:
	bool _enable;
};

// A type specialized property set base class - the objects vector will usually be only one pointer.
// Editing multiple objects at once is allowed.
// For a business object of type 'X', derive your custom property set from base class SetT<X>.
template <typename T>
struct SetT : public Set, public std::vector<T *>
{
	// zero objects
	SetT(bool enable = true) : Set(enable) { };
	// single object edit
	SetT(T *p, bool enable = true) : Set(enable) { push_back(p); };
	// multi-selection edit
	SetT(std::vector<T *> &list, bool enable = true) : Set(enable), std::vector<T*>(list) { };

	void setObject(T *p)
	{
		clear();
		push_back(p);
	}

	void setObjects(std::vector<T *> &list)
	{
		clear();
		assign(list.begin(), list.end());
	}
};

// A type specialized property set base class - the objects vector will usually be only one pointer.
// Editing multiple objects at once is allowed.
// For a business object of type 'X', derive your custom property set from base class SetT<X>.
template <typename T>
struct SetFollowT : public Set, public std::vector<T *>, public IFollow<T*>
{
	sophia::delegate0<void> Remove;

	// zero objects
	SetFollowT(bool enable = true) : Set(enable) { };

	// single object edit
	SetFollowT(T *p, bool enable = true) : Set(enable) { setObject(p); };

	// multi-selection edit
	SetFollowT(std::vector<T *> &list, bool enable = true) : Set(enable), std::vector<T*>(list) 
	{
		if ( size() > 0)
			at(0)->follow(this);
	}

	~SetFollowT()
	{
		clear();
	}

	void clear()
	{
		if ( size() > 0)
			at(0)->ignore(this);
		std::vector<T *>::clear();
	}

	void setObject(T *p)
	{
		clear();
		push_back(p);
		p->follow(this); 
	}

	void setObjects(std::vector<T *> &list)
	{
		clear();
		assign(list.begin(), list.end());
		if ( size() > 0)
			at(0)->follow(this);
	}
	// changed.
	virtual void onChange(T*)
	{
		Set::onChange();
	}
	// removed.
	virtual void onRemove(T* item)
	{
		for (size_t i = 0; i < size(); i++)
		{
			if (at(i) == item)
			{
				erase(begin() + i);
				break;
			}
		}
		if (size() == 0 && !Remove.empty())
			Remove();
	}
};

};