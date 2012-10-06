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

Property::Property(const TCHAR *name, const TCHAR *notes, IControl *control) :
	Name(name), Notes(notes), Control(control)
{
}

Property::~Property()
{
	delete Control;
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
		Columns.push_back(s->Items[i]);
}
