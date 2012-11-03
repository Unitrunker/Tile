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
	Set(bool bReadOnly = false);
	virtual ~Set();
	void Add(Section *);
	void setReadOnly(bool bSet);
	bool getReadOnly() const;
private:
	bool _readOnly;
};

// A type specialized property set base class - the Value accessor allows swapping out object pointers.
// For a business object of type 'X', derive your custom property set from base class SetT<X>.
template <typename T>
struct SetT : public Value<T *>, public Set
{
	SetT(T *p, bool readOnly = false) : Set(readOnly), Value(p) { };
};

};