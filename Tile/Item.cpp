#include "stdafx.h"
#include "Item.h"

using namespace Tiles;

/*
Copyright © 2011 Rick Parrish
*/

_Item::_Item(identity_t id, Theme &theme, const Theme::Font &font, const Combo::Item &item, Combo &combo) :
	_label(item._label),
	_value(item._value),
	Edit(id, theme, font, _label),
	_combo(combo)
{
	setReadOnly(true);
}

_Item::~_Item()
{
}

/// <param name="canvas">canvas where this object will be drawn</param>
bool _Item::Draw(ICanvas *canvas, bool bFocus)
{
	bFocus; // ignored.
	return Edit::Draw(canvas, _focus);
}

// focus
void _Item::setFocus(bool focus)
{
	Edit::setFocus(focus);
}

// key event sink
bool _Item::dispatch(KeyEvent &action)
{
	if ( action._what == KeyEvent::DOWN &&
		(action._code == VK_RETURN || action._code == VK_ESCAPE) )
	{
		_combo.closePopup();
		return true;
	}
	return Edit::dispatch(action);
}

// mouse event sink
bool _Item::dispatch(MouseEvent &action)
{
	if ( Edit::dispatch(action) && action._what == MouseEvent::eDownClick )
	{
		_combo.setValue(_value);
		_combo.closePopup();
		return true;
	}
	return false;
}
