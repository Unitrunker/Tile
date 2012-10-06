#include "stdafx.h"
#include "Check.h"
#include <vector>
#include "ICanvas.h"
#include "Flow.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Check::Check(identity_t id, Theme &theme, IAccessor<bool> *pAccess, align_t align) : 
	Control(id, theme),
	_cursor(0), 
	_checked(false),
	_access(pAccess),
	_align(align)
{
	Theme::Font font = {Theme::eArrow, theme.Arrow};
	_tile.setFont(font);

	FlowDesc desc = {1, 1, 0, true};
	setFlow(eDown, desc);
	setFlow(eRight, desc);
	_fore[0].index = Theme::eDataFore;
	_back[0].index = Theme::eDataBack;
	_fore[1].index = Theme::eCellFore;
	_back[1].index = Theme::eCellBack;
	_glyphs[0].index = Theme::eChecked;
	_glyphs[0].glyph = theme.Checked;
	_glyphs[1].index = Theme::eUncheck;
	_glyphs[1].glyph = theme.Uncheck;
}

Check::Check(identity_t id, Theme &theme, bool &checked, align_t align) : 
	Control(id, theme),
	_cursor(0), 
	_checked(false),
	_access( new Reference<bool>(checked) ),
	_align(align)
{
	Theme::Font font = {Theme::eArrow, theme.Arrow};
	_tile.setFont(font);

	FlowDesc desc = {1, 1, 0, true};
	_tile.setFlow(eDown, desc);
	_tile.setFlow(eRight, desc);
	_fore[0].index = Theme::eDataFore;
	_back[0].index = Theme::eDataBack;
	_fore[1].index = Theme::eCellFore;
	_back[1].index = Theme::eCellBack;
	_glyphs[0].index = Theme::eChecked;
	_glyphs[0].glyph = theme.Checked;
	_glyphs[1].index = Theme::eUncheck;
	_glyphs[1].glyph = theme.Uncheck;
}

/// <param name="canvas">canvas where this object will be drawn</param>
bool Check::Draw(ICanvas *canvas, bool bFocus)
{
	_ASSERT(_access != NULL);
	_checked = _access->getValue();
	bool focus = bFocus && _focus;
	color_t fore = _tile.getColor(_fore[focus]);
	color_t back = _tile.getColor(_back[focus]);
	const string_t &checked = this->_tile.getTheme().getGlyph(_glyphs[_checked]);
	const Font& font = _tile.getFont(_tile.getFont());
	canvas->DrawString(_tile.rect(), _tile.scrollBox(), fore, back, font, _align, checked);
	_tile.setChanged(false);
	return true;
}

// key event sink
bool Check::dispatch(KeyEvent &action)
{
	// key down
	if (action._what == KeyEvent::DOWN)
	{
		// space bar
		if (action._code == VK_SPACE && !_readonly)
		{
			if (_access != NULL)
				_checked = _access->getValue();
			_checked = !_checked;
			if (_access != NULL)
				_access->setValue(_checked);
			if (!Select.empty())
				Select(this, _checked);
			setChanged(true);
			return true;
		}
	}
	return false;
}

// mouse event sink
bool Check::dispatch(MouseEvent &action)
{
	Control::dispatch(action);
	if (action._button == MouseEvent::eLeft &&
		action._what == action.eDownClick)
	{
		if (!_readonly)
		{
			if (_access != NULL)
				_checked = _access->getValue();
			_checked = !_checked;
			if (_access != NULL)
				_access->setValue(_checked);
			if (!Select.empty())
				Select(this, _checked);
			setChanged(true);
			if (!_focus)
				getContainer()->setFocus(this);
		}
	}
	return true;
}

// instance type
const char* Check::getType() const
{
	return Check::type();
}

const char* Check::type()
{
	return "Check";
}

// serialize
bool Check::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	_tile.save(writer);
	writer.writeEndObject(true);
	return true;
}

// Check box specific methods

void Check::setAccessor(IAccessor<bool> *pAccess)
{
	_access = pAccess;
}

void Check::setGlyphs(const TCHAR *checked, const TCHAR *unchecked)
{
	_glyphs[0].index = Theme::eDefault;
	_glyphs[0].glyph = unchecked;
	_glyphs[1].index = Theme::eDefault;
	_glyphs[1].glyph = checked;
}

void Check::setFore(const Theme::Color &normal, const Theme::Color &focus)
{
	_fore[0] = normal;
	_fore[1] = focus;
}

void Check::setBack(const Theme::Color &normal, const Theme::Color &focus)
{
	_back[0] = normal;
	_back[1] = focus;
}

void Check::setAlign(align_t align)
{
	_align = align;
}
