#include "Pane.h"
#include "Button.h"
#include "Edit.h"
#include "Accessor.h"

/*
Copyright © 2011-2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct PickPath : public Pane
{
	PickPath(identity_t id, Theme &theme, const TCHAR *prompt, IAccessor<string_t>* access);
	~PickPath();

	virtual void setRect(const rect_t &rect);

	// readonly
	using Pane::getReadOnly;
	using Pane::setReadOnly;
	// enable
	using Pane::getEnable;
	using Pane::setEnable;

private:
	void click(Button*);

	IAccessor<string_t>* _access;
	const TCHAR* _prompt;
	Edit* _edit;
	Button* _button;
};

};