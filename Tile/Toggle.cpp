#include "stdafx.h"
#include "Toggle.h"
#include "ICanvas.h"
#include "Pane.h"
#include "JSON.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Toggle::Toggle(identity_t id, Theme &theme, Theme::Font& font, std::vector<Item>& list, IAccessor<long> *pAccess) : 
	Control(id, theme, font),
	_access(pAccess),
	_align(eLeft),
	_inside(_ignore),
	_list(list),
	_ignore(0)
{
	Flow desc = {1, 1, 0, true};
	_tile.setFlow(eDown, desc);
	_tile._pSelf = this;
}

Toggle::Toggle(identity_t id, Theme &theme, Theme::Font& font, std::vector<Item>& list, long &value) : 
	Control(id, theme, font),
	_access(&_inside),
	_align(eLeft),
	_inside(value),
	_list(list),
	_ignore(0)
{
	Flow desc = {1, 1, 0, true};
	_tile.setFlow(eDown, desc);
	_tile._pSelf = this;
}

/// <param name="canvas">canvas where this object will be drawn</param>
bool Toggle::Draw(ICanvas *canvas, bool bFocus)
{
	size_t index = 0;
	const Theme& theme = _tile.getTheme();
	long value = _access->getValue();

	while (index < _list.size() && _list[index]._value != value) index++;

	string_t text = (index < _list.size()) ? _list[index]._label : _T("");

	bool focus = bFocus && _focus;
	color_t fore = _foreAccess->getValue(theme, focus);
	color_t back = _backAccess->getValue(theme, focus);
	const Font font = _tile.getFont(_tile.getFont());
	canvas->DrawString(_tile.rect(), _tile.scrollBox(), fore, back, font, _align, text);
	_tile.setChanged(false);
	return true;
}

void Toggle::step()
{
	size_t index = 0;
	long value = _access->getValue();

	if (_readOnly || !_enable)
		return;

	if (_list.size() > 0)
	{
		while (index < _list.size() && _list[index]._value != value) index++;
		index = (index + 1) % _list.size();
		value = _list[index]._value;
		_access->setValue(value);
		setChanged(true);
	}
}

// key event sink
bool Toggle::dispatch(KeyEvent &action)
{
	// key down
	if ( getEnable() && !getReadOnly() &&
		action._what == KeyEvent::DOWN)
	{
		// space bar
		if (action._code == VK_SPACE)
		{
			step();
			return true;
		}
	}
	return false;
}

// mouse event sink
bool Toggle::dispatch(MouseEvent &action)
{
	if (action._button == MouseEvent::eLeft &&
		action._what == action.eDownClick)
	{
		if (getEnable() && !getReadOnly() && _focus)
		{
			step();
		}
		else
		{
			getContainer()->setFocus(this);
		}
		return true;
	}
	return false;
}

// instance type
const char* Toggle::getType() const
{
	return Toggle::type();
}

const char* Toggle::type()
{
	return "Toggle";
}

void Toggle::setAccessor(IAccessor<long>* pAccess)
{
	if (pAccess == NULL)
		_access = &_inside;
	else
		_access = pAccess;
}
