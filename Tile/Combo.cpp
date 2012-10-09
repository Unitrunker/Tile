#include "stdafx.h"
#include "Combo.h"
#include "Item.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

static align_t zero = {0};

Combo::Combo(identity_t id, Theme &theme, Theme::Font &font, std::vector<Item> &list, IAccessor<long>* access) :
	Flow(id, theme, font, eRight), 
	_list(list), 
	_drop(0, theme, eDown), 
	_text(0, theme, font, eLeft, _T("null")),
	_index(list.size()),
	_theme(theme),
	_popup(NULL),
	_popFlow(NULL),
	_focus(false),
	_accessInt(access),
	_accessText(NULL)
{
	meter_t height = theme.Text._height;
	Add(&_text, height, 2048, 1);
	Add(&_drop, 1, 1, 0, 1);
	FlowDesc desc = {1, 1, 0, true};
	setFlow(eDown, desc);
	_fore[0].index = Theme::eDataFore;
	_back[0].index = Theme::eDataBack;
	_fore[1].index = Theme::eCellFore;
	_back[1].index = Theme::eCellBack;
	_text._fore = _fore[_focus];
	_text._back = _back[_focus];
	_thick.local = true;
	_thick.thick = 0;
}

Combo::Combo(identity_t id, Theme &theme, Theme::Font& font, std::vector<Item> &list, IAccessor<string_t>* access) :
	Flow(id, theme, font, eRight), 
	_list(list),
	_drop(0, theme, eDown), 
	_text(0, theme, font, zero, _T("null")),
	_index(list.size()),
	_theme(theme),
	_popup(NULL),
	_popFlow(NULL),
	_focus(false),
	_accessText(access),
	_accessInt(NULL)
{
	meter_t height = theme.Text._height;
	Add(&_text, height, 2048, 1);
	Add(&_drop, 1, 1, 0, 1);
	_fore[0].index = Theme::eDataFore;
	_back[0].index = Theme::eDataBack;
	_fore[1].index = Theme::eCellFore;
	_back[1].index = Theme::eCellBack;
	_text._fore = _fore[_focus];
	_text._back = _back[_focus];
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
	if (action._what == KeyEvent::DOWN)
	{
		switch (action._code)
		{
			case VK_DOWN:
				if ( (_index + 1) < _list.size() )
					setIndex(_index + 1);
				return true;

			case VK_UP:
				if ( _index > 0 )
					setIndex(_index - 1);
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
		if ( _drop.contains(action._place) )
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
				Flow *pFlow = new Flow(0, _theme, eDown);
				for (size_t i = 0; i < size; i++)
				{
					_Item *pItem = new _Item(i, _theme, _tile.getFont(), _list[i], *this);
					pItem->setMin(eRight, rect.wide);
					pItem->setMax(eRight, rect.wide);
					pFlow->Add(pItem, 1, 1, 0, true);
					rect.high += _theme.Text._height;
				}
				pFlow->setIndex(size);
				if (rect.high > 0)
				{
					_popFlow = pFlow;
					_popup = _pDesktop->popup(rect, pFlow, this);
					_popFlow->setIndex(_index);
					_popup->setFocus(_popFlow->getControl(_index));
				}
			}
		}
		if (!_focus)
			getContainer()->setFocus(this);
		return true;
	}
	return false;
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
		_text._fore = _fore[focus];
		_text._back = _back[focus];
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

void Combo::getLabel(string_t &value)
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

void Combo::getValue(long &value)
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
			if (_popFlow != NULL)
			{
				_popFlow->setIndex(_index);
				_popup->setFocus( _popFlow->getControl(_index) );
			}
		}
	}
}

// serialize
bool Combo::save(JSON::Writer &writer)
{
	// TODO!
	return false;
}

bool Combo::Draw(ICanvas *canvas, bool bFocus)
{
	synchronize();
	return Flow::Draw(canvas, bFocus);
}

void Combo::closePopup()
{
	if (_popup)
	{
		_popup->close();
		_popup = NULL;
		_popFlow = NULL;
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

// pre-empt normal Flow logic for calculating horizontal flow

// get/set accessors for layout mimimums
void Combo::getMin(orient_t flow, meter_t &min)
{
	if ( horizontal(flow) )
		_tile.getMin(flow, min);
	else
		Flow::getMin(flow, min);
}

// get/set accessors for layout mimimums
void Combo::getMax(orient_t flow, meter_t &max)
{
	if ( horizontal(flow) )
		_tile.getMax(flow, max);
	else
		Flow::getMax(flow, max);
}

// get/set accessors for layout mimimums
void Combo::getWeight(orient_t flow, meter_t &weight)
{
	Flow::getWeight(flow, weight);
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
