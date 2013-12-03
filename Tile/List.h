#include "Pane.h"
#include "Property.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Text;

// A deceptively simple property list control!
struct List : public Pane
{
	List(identity_t id, Theme &theme);
	virtual ~List();

	// instance type
	virtual const char* getType() const;

	// the property set you provide drives this control.
	void setItems(struct Set *set);
	// clear the property list
	void clear();

	// internal callbacks
	void select(Property*);
	void select(Section*);

private:
	Set* _set;
	Text* _note;
};

};