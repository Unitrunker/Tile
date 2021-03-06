#include "stdafx.h"
#include "Edit.h"
#include <vector>
#include "ICanvas.h"
#include "Pane.h"
#include "JSON.h"

/*
Copyright � 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Edit::Edit(identity_t id, Theme &theme, const Theme::Font &font, IAccessor<string_t> *pAccess) :
	Control(id, theme, font),
	_cursor(0), 
	_access(pAccess),
	_align(eLeft),
	_local(true),
	_edit(false)
{
	Flow desc = {1, 1, 0, true};
	setFlow(eDown, desc);
	_foreAccess = &_foreLocal;
	_backAccess = &_backLocal;
}

Edit::Edit(identity_t id, Theme &theme, const Theme::Font &font, string_t &text) :
	Control(id, theme, font),
	_cursor(0), 
	_access(new Reference<string_t>(text)),
	_align(eLeft),
	_local(true),
	_edit(false)
{
	Flow desc = {1, 1, 0, true};
	setFlow(eDown, desc);
}

// TODO: I've removing the edit control's bounding box. I'll revisit 
// this later when a generic border style mechanism is added.
/// <param name="canvas">canvas where this object will be drawn</param>
bool Edit::Draw(ICanvas *canvas, bool bFocus)
{
	rect_t rect = {0}, box = {0};
	Theme &theme = _tile.getTheme();

	_tile.getRect(rect);
	_tile.getScrollBox(box);

	bool focus = bFocus && _focus;

	color_t fore = _foreAccess->getValue(theme, focus);
	color_t back = _backAccess->getValue(theme, focus);
	if ( getEnable() && !getReadOnly() && focus)
	{
		canvas->DrawEditString(rect, box, fore, back, theme.Text, _align, _text, getIndex());
	}
	else
	{
		const string_t &text = _access == NULL ? _text : _access->getValue();
		canvas->DrawString(rect, box, fore, back, theme.Text, _align, text);
	}
	setChanged(false);
	return true;
}

bool Edit::onCursorLeft()
{
	size_t cursor = getIndex();
	if (cursor > 0)
	{
		setIndex(cursor - 1);
		return true;
	}
	return false;
}

bool Edit::onCursorRight()
{
	size_t cursor = getIndex();
	if (cursor < _text.size())
	{
		setIndex(cursor + 1);
		return true;
	}
	return false;
}

bool Edit::onChar(TCHAR ch)
{
	size_t cursor = getIndex();
	if (cursor > _text.size())
		cursor = _text.size();
	_edit = true;
	_text.insert(cursor, 1, ch);
	cursor++;
	setIndex(cursor);
	return true;
}

bool Edit::onBackspace()
{
	size_t cursor = getIndex();
	if (cursor > 0 && cursor <= _text.size())
	{
		cursor--;
		_text.erase(cursor, 1);
		_edit = true;
		setIndex(cursor);
	}
	return true;
}

bool Edit::onDelete()
{
	size_t cursor = getIndex();
	if (cursor < _text.size() )
	{
		_text.erase(cursor, 1);
		_edit = true;
		setChanged(true);
	}
	return true;
}

// key event sink
// TODO: assumes US keyboard mappings for VK_OEM_1 through VK_OEM_7.
bool Edit::dispatch(KeyEvent &action)
{
	// key down
	if ( getEnable() && !getReadOnly() && 
		action._what == KeyEvent::DOWN )
	{
		// 
		switch (action._code)
		{
			case VK_HOME:
				setIndex(0);
				return true;

			case VK_END:
				setIndex(_text.size());
				return true;

			case VK_LEFT:
				// TODO: handle CONTROL modifier
				return onCursorLeft();

			case VK_RIGHT:
				// TODO: handle CONTROL modifier
				return onCursorRight();

			case VK_BACK:
				return onBackspace();

			case VK_DELETE:
				return onDelete();

			case VK_ESCAPE:
				if (_access != NULL)
				{
					_text = _access->getValue();
					_edit = false;
				}
				if (getIndex() > _text.size())
					setIndex(_text.size());
				// return false so escape also processed further up hierarchy.
				return false;

			case VK_RETURN:
				apply();
				return false;

			case VK_DOWN:
			case VK_UP:
			case VK_TAB:
				return false;

			case VK_NUMPAD0:
			case VK_NUMPAD1:
			case VK_NUMPAD2:
			case VK_NUMPAD3:
			case VK_NUMPAD4:
			case VK_NUMPAD5:
			case VK_NUMPAD6:
			case VK_NUMPAD7:
			case VK_NUMPAD8:
			case VK_NUMPAD9:
			{
				TCHAR ch = static_cast<TCHAR>(action._code) - VK_NUMPAD0 + '0';
				return onChar(ch);
			}

			case VK_SPACE:
				return onChar(' ');

			case VK_OEM_PLUS:
				return action._mask & KeyEvent::SHIFT ? 
					onChar('+') : onChar('=');

			case VK_ADD:
				return onChar('+');

			case VK_OEM_MINUS:
				return action._mask & KeyEvent::SHIFT ? 
					onChar('_') : onChar('-');

			case VK_SUBTRACT:
				return onChar('-');

			case VK_OEM_PERIOD:
				return action._mask & KeyEvent::SHIFT ? 
					onChar('>') : onChar('.');

			case VK_DECIMAL:
				return onChar('.');

			case VK_DIVIDE:
				return onChar('/');

			case VK_MULTIPLY:
				return onChar('*');

			case VK_OEM_COMMA:
				return action._mask & KeyEvent::SHIFT ? 
					onChar('<') : onChar(',');

			case VK_OEM_1: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar(':') : onChar(';');

			case VK_OEM_2: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar('?') : onChar('/');

			case VK_OEM_3: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar('~') : onChar('`');

			case VK_OEM_4: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar('{') : onChar('[');

			case VK_OEM_5: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar('|') : onChar('\\');

			case VK_OEM_6: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar('}') : onChar(']');

			case VK_OEM_7: // US keyboards.
				return action._mask & KeyEvent::SHIFT ? 
					onChar('"') : onChar('\'');

			case '0':
				return action._mask & KeyEvent::SHIFT ? 
					onChar(')') : onChar('0');
			case '1':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('!') : onChar('1');
			case '2':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('@') : onChar('2');
			case '3':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('#') : onChar('3');
			case '4':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('$') : onChar('4');
			case '5':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('%') : onChar('5');
			case '6':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('^') : onChar('6');
			case '7':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('&') : onChar('7');
			case '8':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('*') : onChar('8');
			case '9':
				return action._mask & KeyEvent::SHIFT ? 
					onChar('(') : onChar('9');

			case VK_NEXT:
			case VK_PRIOR:
				return false;

				// direct ASCII mapped key codes.
			default:
				// no control or ALT modifier?
				if ( (action._mask & (KeyEvent::CONTROL|KeyEvent::ALT)) == 0 )
				{
					// upper/lower case?
					TCHAR ch = static_cast<TCHAR>(action._code);
					// ASCII
					if (ch >= 'A' && ch <= 'Z')
					{
						bool bShift = (action._mask & KeyEvent::SHIFT) != 0;
						bool bCaps = (action._mask & KeyEvent::CAPS) != 0;
						if (!bShift ^ bCaps)
							ch += 32;
						return onChar(ch);
					}
				}
				break;
		}
	}
	return false;
}

// mouse event sink
bool Edit::dispatch(MouseEvent &action)
{
	if ( Control::dispatch(action) )
		return true;

	if ( action._what == MouseEvent::eDownClick)
	{
		if (!_focus)
			getContainer()->setFocus(this);
		return true;
	}
	return false;
}

/// <param name="focus">true if this control has focus</param>
void Edit::setFocus(bool focus)
{
	bool change = _focus ^ focus;
	if (change)
	{
		_focus = focus;
		if (_focus)
		{
			getContainer()->Shown(this);
			if (_access != NULL)
			{
				_text = _access->getValue();
				_edit = false;
			}
		}
		else
		{
			apply();
		}
		setChanged(change);
	}
}

// force an update of any edits in-progress.
void Edit::apply()
{
	if ( _edit && getEnable() && !getReadOnly() )
	{
		if (_access == NULL)
		{
			if (!Select.empty())
				Select(this, _text);
		}
		else if ( _access->getValue().compare(_text) != 0)
		{
			_access->setValue(_text);
			if (!Select.empty())
				Select(this, _text);
		}
	}
	_edit = false;
}

// instance type
const char* Edit::getType() const
{
	return Edit::type();
}

const char* Edit::type()
{
	return "Edit";
}

void Edit::setAccessor(IAccessor<string_t> *pAccess)
{
	_access = pAccess;
}

void Edit::setLocal(bool local)
{
	_local = local;
}

void Edit::setIndex(size_t cursor)
{
	if (cursor != getIndex())
	{
		if (_local)
			_cursor = cursor;
		else
			getContainer()->setIndex(cursor);

		setChanged(true);
	}
}

size_t Edit::getIndex() const
{
	if (_local)
		return _cursor;

	return getContainer()->getIndex();
}

// hover
void Edit::setHover(bool hover)
{
	_hover = hover;
}
