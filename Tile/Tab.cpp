#include "stdafx.h"
#include "Tab.h"
#include "Pane.h"
#include "ICanvas.h"
#include "JSON.h"
#include "Text.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Tab::Tab(identity_t id, Theme& theme, Theme::Font &font, const TCHAR *text, const TCHAR *tip) :
	Control(id, theme, font),
	_text(text),
	_popup(NULL)
{
	_colorIdle[0]._index = Theme::eToolFore;
	_colorIdle[0]._color = RGB(0, 0, 0);
	_colorIdle[1]._index = Theme::eToolBack;
	_colorIdle[1]._color = RGB(192, 192, 192);

	_colorOver[0]._index = Theme::eToolFore;
	_colorOver[0]._color = RGB(0, 0, 0);
	_colorOver[1]._index = Theme::eToolOver;
	_colorOver[1]._color = RGB(255, 255, 255);

	_colorSelect[0]._index = Theme::eToolFore;
	_colorSelect[0]._color = RGB(0, 0, 0);
	_colorSelect[1]._index = Theme::eToolOver;
	_colorSelect[1]._color = RGB(255, 255, 255);

	Flow high = {1, 1, 0, true};
	Flow wide = {0, 4096, 1, false};
	setFlow(eDown, high);
	setFlow(eRight, wide);
	if (tip)
		setTip(tip);
}

Tab::~Tab()
{
	if (_popup)
		_popup->close();
	_popup = NULL;
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Tab::Draw(ICanvas *canvas, bool bFocus)
{
	align_t align = eLeft|eRight|eUp|eDown;

	bFocus;

	rect_t rect = _tile.rect();
	rect_t box = _tile.scrollBox();

	color_t fore = 0;
	color_t back = 0;

	Pane *pane = getContainer();
	size_t index = pane->getIndex();
	bool focus = pane->getControl(index) == static_cast<IControl*>(this);

	if (focus)
	{
		fore = _tile.getColor(_colorSelect[0]);
		back = _tile.getColor(_colorSelect[1]);
	}
	else if (_hover)
	{
		fore = _tile.getColor(_colorOver[0]);
		back = _tile.getColor(_colorOver[1]);
	}
	else
	{
		fore = _tile.getColor(_colorIdle[0]);
		back = _tile.getColor(_colorIdle[1]);
	}
	const Font& font = _tile.getFont(_tile.getFont());
	// draw text.
	canvas->DrawString(rect, box, fore, back, font, align, _text);
	_tile.setChanged(false);
	return true;
}

// instance type
const char* Tab::getType() const
{
	return Tab::type();
}

const char* Tab::type()
{
	return "Tab";
}

// key event sink
bool Tab::dispatch(KeyEvent &action)
{
	if ( getEnable() && !getReadOnly() && action._code == VK_SPACE )
	{
		switch (action._what)
		{
			case KeyEvent::DOWN:
				return true;

			case KeyEvent::UP:
				_pDesktop->setFocus(this);
				setChanged(true);
				if (!Click.empty())
					Click(this);
				return true;

			default:
				break;
		}
	}
	return false;
}

// mouse event sink
bool Tab::dispatch(MouseEvent &action)
{
	if (action._button == MouseEvent::eLeft)
	{
		switch (action._what)
		{
			case MouseEvent::eDownClick:
				return true;

			case MouseEvent::eUpClick:
				if ( getEnable() && !getReadOnly() )
				{
					setFocus(true);
					setChanged(true);
					if (!Click.empty())
						Click(this);
				}
				return true;

			default:
				break;
		}
	}
	return Control::dispatch(action);
}

// serialize
bool Tab::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	_tile.save(writer);

	writer.writeStartNamedObject("Idle");
	 saveColor(writer, "fore", _colorIdle[true], false);
	 saveColor(writer, "back", _colorIdle[false], false);
	writer.writeEndObject(false);

	writer.writeStartNamedObject("Over");
	 saveColor(writer, "fore", _colorOver[true], false);
	 saveColor(writer, "back", _colorOver[false], false);
	writer.writeEndObject(false);

	writer.writeStartNamedObject("Select");
	 saveColor(writer, "fore", _colorSelect[true], false);
	 saveColor(writer, "back", _colorSelect[false], false);
	writer.writeEndObject(false);

	writer.writeNamedValue("text", _text.c_str());
	saveFont(writer, "Font", _tile.getFont(), false);
	writer.writeEndObject(true);
	return true;
}

// de-serialize
bool Tab::load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pTab)
{
	bool bOK = false;
	if ( !strcmp(type, Tab::type()) )
	{
		identity_t id = 0;
		std::string text;
		Flow horz, vert;
		Theme::Color idle[2];
		Theme::Color over[2];
		Theme::Color focus[2];
		Theme::Font desc = {Theme::eDefault, Font(_T("MS Shell Dlg"), 18, 0)};

		do
		{
			bOK = reader.namedValue("id", id) ||
				loadFlow(reader, "Horz", horz) ||
				loadFlow(reader, "Vert", vert) ||
				reader.namedValue("text", text) ||
				loadFont(reader, "Font", desc);

			if (!bOK)
			{
				if ( reader.beginNamedObject("Idle") )
				{
					do
					{
						bOK = loadColor(reader, "fore", idle[true]) ||
							loadColor(reader, "back", idle[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
				else if ( reader.beginNamedObject("Over") )
				{
					do
					{
						bOK = loadColor(reader, "fore", over[true]) ||
							loadColor(reader, "back", over[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
				else if ( reader.beginNamedObject("Select") )
				{
					do
					{
						bOK = loadColor(reader, "fore", focus[true]) ||
							loadColor(reader, "back", focus[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
			}
		}
		while (bOK && reader.comma());
		if (bOK)
		{
			string_t wide;
			wide.resize(text.size() + 1, ' ');
			size_t used = 0;
			mbstowcs_s(&used, &wide[0], wide.size(), text.c_str(), text.size());
			wide.resize(used);

			Tab *p = new Tab(id, theme, desc, wide.c_str());
			p->setFlow(eRight, horz);
			p->setFlow(eDown, vert);
			p->_colorIdle[false] = idle[false];
			p->_colorIdle[true] = idle[true];
			p->_colorOver[false] = over[false];
			p->_colorOver[true] = over[true];
			p->_colorSelect[false] = focus[false];
			p->_colorSelect[true] = focus[true];
			pTab = p;
		}
	}
	return bOK;
}

bool Tab::getTip(string_t &tip) const
{
	if (_tip.size() > 0)
	{
		tip = _tip;
		return true;
	}
	return false;
}

void Tab::setTip(const TCHAR *tip)
{
	_tip = tip;
}

// hover
void Tab::setHover(bool hover)
{
	Control::setHover(hover);
	if (hover && _tile.getTheme().Tips && _tip.size())
	{
		size_t size = (5 * _tip.size() + 7) / 8;
		Flow vert = {1, 1, 0, true};
		Flow horz = { size, size, 0, true };
		Theme::Font font = { Theme::eText, _tile.getTheme().Text };
		Pane *pane = new Pane(0, _tile.getTheme(), eDown);
		Text *text = new Text(0, _tile.getTheme(), font, eLeft, _tip.c_str());
		text->setFlow(eRight, horz);
		text->setFlow(eDown, vert);
		rect_t rect;
		_ASSERT(_popup == NULL);
		pane->Add(text);

		// TODO: better placement.
		rect.wide = size * _tile.getTheme().Text._height + 2;
		rect.high = _tile.getTheme().Text._height + 2;

		pane->setBorder(1);

		rect_t here;
		_tile.getRect(here);

		rect.x = here.left();
		rect.y = here.bottom();

		_popup = _pDesktop->popup(rect, pane, this);
	}
	else if (_popup)
	{
		_popup->close();
		_popup = NULL;
	}
}

void Tab::setColorIdle(const Theme::Color &fore, const Theme::Color &back)
{
	_colorIdle[0] = fore;
	_colorIdle[1] = back;
}

void Tab::setColorOver(const Theme::Color &fore, const Theme::Color &back)
{
	_colorOver[0] = fore;
	_colorOver[1] = back;
}

void Tab::setColorSelect(const Theme::Color &fore, const Theme::Color &back)
{
	_colorSelect[0] = fore;
	_colorSelect[1] = back;
}

#if 0
bool Tab::Add(const TCHAR *label, sophia::delegate1<void, Tab*> &click, const TCHAR *tip)
{
	Theme &theme = _tile.getTheme();
	Theme::Font font = { Theme::eText, theme.Text };
	return Add(label, font, click, tip);
}

bool Tab::Add(const TCHAR *label, Theme::Font &font, sophia::delegate1<void, Tab*> &click, const TCHAR *tip)
{
	Theme &theme = _tile.getTheme();
	Flow flow = {1, 1, 0, true};
	Tab *Tab = new Tab(0, theme, font, label);
	Tab->setFlow(eDown, flow);
	Tab->Click = click;
	Pane::Add(Tab, 0, 4096, 1, false);
	return true;
}
#endif