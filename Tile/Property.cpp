#include "stdafx.h"
#include "Property.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Section::Section(const TCHAR *caption, const TCHAR *notes) :
	Caption(caption), Notes(notes)
{
}

Section::~Section()
{
	size_t i = Items.size();
	while (i)
	{
		delete Items[--i];
	}
	Items.clear();
}

void Section::Add(Property *p)
{
	Items.push_back(p);
}

Property::Property(const TCHAR *name, const TCHAR *notes, IControl *control, bool bGrid, bool bList) :
	Name(name), Notes(notes), Control(control), list(bList), grid(bGrid)
{
}

Property::~Property()
{
	delete Control;
}

Set::Set(bool readOnly) : _readOnly(readOnly)
{
}

Set::~Set()
{
	Columns.clear();
	size_t i = Sections.size();
	while (i)
	{
		delete Sections[--i];
	}
	Sections.clear();
}

void Set::Add(Section *s)
{
	Sections.push_back(s);
	for (size_t i = 0; i < s->Items.size(); i++)
	{
		Property *prop = s->Items[i];
		prop->Control->setReadOnly(_readOnly);
		Columns.push_back(prop);
	}
}

void Set::setReadOnly(bool bSet)
{
	for (size_t i = 0; i < Columns.size(); i++)
	{
		Columns[i]->Control->setReadOnly(bSet);
	}
	_readOnly = bSet;
}

bool Set::getReadOnly() const
{
	return _readOnly;
}