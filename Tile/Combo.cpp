#include "stdafx.h"
#include "Combo.h"
#include "Item.h"
#include "../JSON/Writer.h"

/*
Copyright � 2011, 2012 Rick Parrish
*/

using namespace Tiles;

static align_t zero = {0};

Combo::Combo(identity_t id, Theme &theme, Theme::Font &font, Item list[], size_t size, IAccessor<long>* access) :
	Pane(id, theme, font, eRight), 
	_drop(0, theme, eDown), 
	_text(0, theme, font, eLeft, _T("null")),
	_index(size),
	_theme(theme),
	_popup(NULL),
	_popPane(NULL),
	_focus(false),
	_accessInt(access),
	_accessText(NULL)
{
	for (size_t i = 0; i < size; i++)
	{
		_list.push_back(list[i]);
	}
	meter_t height = theme.Text._height;
	Add(&_text, height, 2048, 1);
	Add(&_drop, 1, 1, 0, 1);
	Flow desc = {1, 1, 0, true};
	setFlow(eDown, desc);
	_text._fore = _foreView;
	_text._back = _backView;
	_thick.local = true;
	_thick.thick = 0;
}

Combo::Combo(identity_t id, Theme &theme, Theme::Font &font, std::vector<Item> &list, IAccessor<long>* access) :
	Pane(id, theme, font, eRight), 
	_list(list), 
	_drop(0, theme, eDown), 
	_text(0, theme, font, eLeft, _T("null")),
	_index(list.size()),
	_theme(theme),
	_popup(NULL),
	_popPane(NULL),
	_focus(false),
	_accessInt(access),
	_accessText(NULL)
{
	meter_t height = theme.Text._height;
	Add(&_text, height, 2048, 1);
	Add(&_drop, 1, 1, 0, 1);
	Flow desc = {1, 1, 0, true};
	setFlow(eDown, desc);
	_text._fore = _foreView;
	_text._back = _backView;
	_thick.local = true;
	_thick.thick = 0;
}

Combo::Combo(identity_t id, Theme &theme, Theme::Font& font, std::vector<Item> &list, IAccessor<string_t>* access) :
	Pane(id, theme, font, eRight), 
	_list(list),
	_drop(0, theme, eDown), 
	_text(0, theme, font, zero, _T("null")),
	_index(list.size()),
	_theme(theme),
	_popup(NULL),
	_popPane(NULL),
	_focus(false),
	_accessText(access),
	_accessInt(NULL)
{
	meter_t height = theme.Text._height;
	Add(&_text, height, 2048, 1);
	Add(&_drop, 1, 1, 0, 1);
	_text._fore = _foreView;
	_text._back = _backView;
	_thick.local = true;
	_thick.thick = 0;
}

Combo::~Combo()
{
	if (_popup)
		_popup->close();
	clear();
}

// instance type
const char* Combo::getType() const
{
	return Combo::type();
}

const char* Combo::type()
{
	return "Combo";
}

// key event sink
bool Combo::dispatch(KeyEvent &action)
{
	if ( getEnable() && !getReadOnly() &&
		action._what == KeyEvent::DOWN )
	{
		switch (action._code)
		{
			case VK_DOWN:
				// Use of up/down arrow conflicts with form navigation.
				// limit use to only when popup is visible.
				if (_popup != NULL)
				{
					if ( (_index + 1) < _list.size() )
						setIndex(_index + 1);
					return true;
				}
				break;
			case VK_UP:
				// Use of up/down arrow conflicts with form navigation.
				// limit use to only when popup is visible.
				if (_popup != NULL)
				{
					if ( _index > 0 )
						setIndex(_index - 1);
					return true;
				}
				break;
			case VK_SPACE:
				if ( _list.size() > 0 )
					setIndex( (_index + 1) % _list.size() );
				return true;
			case VK_RETURN:
				closePopup();
				return true;

			case VK_ESCAPE:
				closePopup();
				break;

			default:
				break;
		}
	}
	return false;
}

// mouse event sink
bool Combo::dispatch(MouseEvent &action)
{
	if (action._what == MouseEvent::eDownClick &&
		action._button == MouseEvent::eLeft)
	{
		// don't show pop-up if we're read-only.
		if ( getEnable() && !getReadOnly() && _drop.contains(action._place) )
		{
			if (_popup != NULL)
			{
				closePopup();
			}
			else
			{
				rect_t rect = {0};
				getRect(rect);
				rect.y = rect.bottom();
				rect.high = 0;

				size_t size = _list.size();
				Pane *pPane = new Pane(0, _theme, eDown);
				for (size_t i = 0; i < size; i++)
				{
					_Item *pItem = new _Item(i, _theme, _tile.getFont(), _list[i], *this);
					pItem->setMin(eRight, rect.wide);
					pItem->setMax(eRight, rect.wide);
					pPane->Add(pItem, 1, 1, 0, true);
					rect.high += _theme.Text._height;
					if (i > 0)
						rect.high += _theme.GridThick;
				}
				pPane->setIndex(size);
				if (rect.high > 0)
				{
					_popPane = pPane;
					_popup = _pDesktop->popup(rect, pPane, this);
					_popPane->setIndex(_index);
					_popup->setFocus(_popPane->getControl(_index));
				}
			}
		}
		if (!_focus)
			getContainer()->setFocus(this);
		return true;
	}
	return Pane::dispatch(action);
}

// focus
bool Combo::getFocus() const
{
	return _focus;
}

void Combo::setFocus(bool focus)
{
	bool change = focus ^ _focus;
	if (change)
	{
		_focus = focus;
		_text._fore = focus ? _foreEdit : _foreView;
		_text._back = focus ? _backEdit : _backView;
		if (focus)
			getContainer()->Shown(this);
		setChanged(true);
		if (!_focus && _popup != NULL)
			closePopup();
	}
}

// hover
void Combo::setHover(bool hover)
{
	hover; // ignored.
}

// navigation
bool Combo::onNear(orient_t)
{
	if (_focus)
		return false;
	getContainer()->setFocus(this);
	return _focus;
}

bool Combo::onFar(orient_t)
{
	if (_focus)
		return false;
	getContainer()->setFocus(this);
	return _focus;
}

void Combo::getLabel(string_t &value) const
{
	if (_index < _list.size())
		value = _list[_index]._label;
}

bool Combo::setLabel(const string_t &value)
{
	_index = _list.size();
	for (size_t i = 0; i < _index; i++)
	{
		if ( value.compare(_list[i]._label) == 0 )
		{
			setIndex(i);
		}
	}
	bool bOK = _index < _list.size();
	if (!bOK)
		_text._text = _T("(null)");
	return bOK;
}

void Combo::getValue(long &value) const
{
	if (_index < _list.size())
		value = _list[_index]._value;
}

bool Combo::setValue(long value)
{
	_index = _list.size();
	for (size_t i = 0; i < _index; i++)
	{
		if (_list[i]._value == value)
		{
			setIndex(i);
		}
	}
	bool bOK = _index < _list.size();
	if (!bOK)
		_text._text = _T("(null)");
	return bOK;
}

void Combo::setIndex(size_t index)
{
	bool change = _index != index;
	if (change)
	{
		_index = index;
		if (_accessInt)
			_accessInt->setValue(_list[index]._value);
		if (_accessText)
			_accessText->setValue(_list[index]._label);
		if (_index < _list.size())
		{
			_text._text = _list[_index]._label;
			_text.setChanged(true);
			if (_popPane != NULL)
			{
				_popPane->setIndex(_index);
				_popup->setFocus( _popPane->getControl(_index) );
			}
		}
	}
}

// serialize
bool Combo::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", "Combo");
	_tile.save(writer);
	writer.writeEndObject(true);
	return true;
}

bool Combo::Draw(ICanvas *canvas, bool bFocus)
{
	synchronize();
	return Pane::Draw(canvas, bFocus);
}

void Combo::closePopup()
{
	if (_popup)
	{
		_popup->close();
		_popup = NULL;
		_popPane = NULL;
	}
}

void Combo::synchronize()
{
	// integer accessor?
	if (_accessInt)
	{
		const long value = _accessInt->getValue();
		// index valid?
		if (_index < _list.size())
		{
			// yes: value matches?
			if (_list[_index]._value == value)
				// yes: do nothing.
				return;
		}
		// update value
		setValue(value);
	}
	// string accessor?
	else if (_accessText)
	{
		const string_t &label = _accessText->getValue();
		// index valid?
		if (_index < _list.size())
		{
			// label matches?
			if ( label.compare(_list[_index]._label) == 0 )
				// yes: do nothing.
				return;
		}
		// update label.
		setLabel(label);
	}
}

// pre-empt normal Pane logic for calculating horizontal flow

// get/set accessors for layout mimimums
void Combo::getMin(orient_t flow, meter_t &min) const
{
	if ( horizontal(flow) )
		_tile.getMin(flow, min);
	else
		Pane::getMin(flow, min);
}

// get/set accessors for layout mimimums
void Combo::getMax(orient_t flow, meter_t &max) const
{
	if ( horizontal(flow) )
		_tile.getMax(flow, max);
	else
		Pane::getMax(flow, max);
}

// get/set accessors for layout mimimums
void Combo::getWeight(orient_t flow, meter_t &weight) const
{
	Pane::getWeight(flow, weight);
}

// Modified getMin above causes standard Flow::setRect to break so use
// this greatly simplified logic for layout of the text and drop arrow.
void Combo::setRect(const rect_t &rect)
{
	_tile.setRect(rect);
	rect_t arrow = rect;
	rect_t text = rect;
	_drop.getMin(eRight, arrow.wide);
	if (rect.wide > arrow.wide)
	{
		text.wide -= arrow.wide;
		arrow.x += text.wide;
		meter_t space = _tile.getThick(_space);
		if (text.wide >= space)
			text.wide -= space;
	}
	else
	{
		if (arrow.wide > rect.wide)
			arrow.wide = rect.wide;
		text.wide = 0;
	}
	_text.setRect(text);
	_drop.setRect(arrow);
}
