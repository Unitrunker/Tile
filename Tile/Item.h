#include "Combo.h"
#include "Edit.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

// This control is strictly for the Combo box. It's used as a list box item.
struct _Item : public Edit
{
	_Item(identity_t id, Theme &theme, const Theme::Font &font, const Combo::Item &item, Combo &combo);
	virtual ~_Item();
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// focus
	virtual void setFocus(bool);
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);

private:
	Combo& _combo;
	string_t _label;
	long _value;
};

};