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

Set::Set(bool enable) : _enable(enable)
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
		prop->Control->setEnable(_enable);
		Columns.push_back(prop);
	}
}

void Set::setEnable(bool bSet)
{
	for (size_t i = 0; i < Columns.size(); i++)
	{
		Columns[i]->Control->setEnable(bSet);
	}
	_enable = bSet;
}

bool Set::getEnable() const
{
	return _enable;
}

void Set::onChange()
{
	for (size_t i = 0; i < Columns.size(); i++)
	{
		Columns[i]->Control->setChanged(true);
	}
}

void Set::apply()
{
	for (size_t i = 0; i < Columns.size(); i++)
	{
		IControl* control = Columns[i]->Control;
		if ( control->getFocus() )
		{
			control->apply();
			break;
		}
	}
}
