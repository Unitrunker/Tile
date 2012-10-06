#include "stdafx.h"
#include "Toggle.h"
#include "ICanvas.h"
#include "Flow.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011 Rick Parrish
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
	FlowDesc desc = {1, 1, 0, true};
	_tile.setFlow(eDown, desc);
	_tile._pSelf = this;
	_fore[0].index = Theme::eDataFore;
	_back[0].index = Theme::eDataBack;
	_fore[1].index = Theme::eCellFore;
	_back[1].index = Theme::eCellBack;
}

Toggle::Toggle(identity_t id, Theme &theme, Theme::Font& font, std::vector<Item>& list, long &value) : 
	Control(id, theme, font),
	_access(&_inside),
	_align(eLeft),
	_inside(value),
	_list(list),
	_ignore(0)
{
	FlowDesc desc = {1, 1, 0, true};
	_tile.setFlow(eDown, desc);
	_tile._pSelf = this;
	_fore[0].index = Theme::eDataFore;
	_back[0].index = Theme::eDataBack;
	_fore[1].index = Theme::eCellFore;
	_back[1].index = Theme::eCellBack;
}

/// <param name="canvas">canvas where this object will be drawn</param>
bool Toggle::Draw(ICanvas *canvas, bool bFocus)
{
	size_t index = 0;
	long value = _access->getValue();

	while (index < _list.size() && _list[index]._value != value) index++;

	string_t text = (index < _list.size()) ? _list[index]._label : _T("");

	bool focus = bFocus && _focus;
	color_t fore = _tile.getTheme().getColor(_fore[focus]);
	color_t back = _tile.getTheme().getColor(_back[focus]);
	const Font font = _tile.getFont(_tile.getFont());
	canvas->DrawString(_tile.rect(), _tile.scrollBox(), fore, back, font, _align, text);
	_tile.setChanged(false);
	return true;
}

void Toggle::step()
{
	size_t index = 0;
	long value = _access->getValue();

	if (_readonly)
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
	if (action._what == KeyEvent::DOWN)
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
		if (_focus)
		{
			step();
		}
		else
		{
			getContainer()->setFocus(this);
		}
	}
	return true;
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

// serialize
bool Toggle::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	_tile.save(writer);
	writer.writeEndObject(true);
	return true;
}

void Toggle::setAccessor(IAccessor<long>* pAccess)
{
	if (pAccess == NULL)
		_access = &_inside;
	else
		_access = pAccess;
}
