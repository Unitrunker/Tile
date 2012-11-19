#include "stdafx.h"
#include "List.h"
#include "Text.h"
#include "Edit.h"
#include "Check.h"
#include "Tree.h"
#include "JSON.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

// _Section describes a row in a List control.
// Expands and collapses items just like a Tree control.
struct _Section : public Tree
{
	_Section(identity_t id, Theme &theme, const Theme::Font &font);
	virtual ~_Section();

	// instance type
	virtual const char* getType() const;
	void setItems(Section *section);

protected:
	virtual void setFocus(IControl *pControl);

private:
	Section* _section;
};

_Section::_Section(identity_t id, Theme &theme, const Theme::Font &font) :
	Tree(id, theme, font), _section(NULL)
{
	_checked = true;
}

_Section::~_Section()
{
	// The property set's controls belong to the property set so
	// remove the property set's controls from the Pane's tile list.
	// Otherwise the Pane's descructor presumes responsibility for deleting them.
	hideControls();
}

// instance type
const char* _Section::getType() const { return "Section"; }

void _Section::setItems(Section *section)
{
	_section = section;

	_label._text = section->Caption;
	Theme& theme = _tile.getTheme();
	Flow text = {0, 4096, 1, false};
	Flow edit = {0, 4096, 3, false};
	Flow line = {1, 1, 0, true};
	Theme::Font textFont = {Theme::eText, theme.Text};

	size_t size = _section->Items.size();
	for (size_t j = 0; j < size; j++)
	{
		Property *item = _section->Items[j];
		Pane *pItem = new Pane(0, theme, eRight);
		Text *pLabel = new Text(0, theme, textFont, eRight, item->Name);
		pLabel->setFlow(eRight, text);
		IControl *pControl = item->Control;
		pControl->setFlow(eRight, edit);
		pItem->Add(pLabel);
		pItem->Add(pControl);
		pItem->setFlow(eDown, line);
		Add(pItem);
	}
}

void _Section::setFocus(IControl *pControl)
{
	Pane::setFocus(pControl);

	if ( getIndex() == 0 )
	{
		// tell property list control to update the descriptive text section.
		Pane *pane = getContainer();
		List *list = static_cast<List*>(pane);
		list->select(_section);
	}
	else if (pControl != NULL)
	{
		size_t size = _section->Items.size();
		size_t i = 0;
		for (i = 0; i < size; i++)
		{
			// Is this control our property's control?
			if (pControl == _section->Items[i]->Control ||
				// Property's control may be composite control 
				// so check the focus control's container too.
				pControl->getContainer() == _section->Items[i]->Control)
					// yes: found it!
					break;
		}
		if (i < size)
		{
			// tell property list control to update the descriptive text section.
			Pane *pane = getContainer();
			List *list = static_cast<List*>(pane);
			list->select(_section->Items[i]);
		}
	}
}

//

List::List(identity_t id, Theme &theme) : 
	Pane(id, theme, eDown),
	_set(NULL), 
	_note(NULL)
{
	_scroll = true;
	_thick.local = true;
	_thick.thick = 0;

	Theme::Font textFont = {Theme::eText, theme.Text};
	Flow text = {0, 4096, 1, false};
	_note = new Text(0, theme, textFont, eLeft|eUp, _T("Some interesting words go here."));
	_note->setFlow(eDown, text);
}

List::~List()
{
	clear();
}

// instance type
const char* List::getType() const
{
	return "List";
}

void List::clear()
{
	size_t size = _listControls.size();
	if (size > 0)
	{
		_listControls.clear();
		_note->_text = _T("Empty");
		_note->setChanged(true);
	}
}

void List::setItems(struct Set *set)
{
	_set = set;

	Pane::clear();

	if (_set)
	{
		Theme& theme = _tile.getTheme();
		Theme::Font textFont = {Theme::eText, theme.Text};
		for (size_t i = 0; i < _set->Sections.size(); i++)
		{
			_Section *pSection = new _Section(0, theme, textFont);
			pSection->setItems(_set->Sections[i]);
			Add(pSection, true);
		}
	}
	Add(_note);
}

void List::select(Property* p)
{
	_note->_text = p->Notes;
	_note->setChanged(true);
}

void List::select(Section* s)
{
	_note->_text = s->Notes;
	_note->setChanged(true);
}

// serialize
bool List::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	_tile.save(writer);
	writer.writeEndObject(true);
	return true;
}

// de-serialize
bool List::load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pControl)
{
	bool bOK = false;
	if ( !strcmp(type, List::type()) )
	{
		identity_t id = 0;
		Flow horz, vert;
		do
		{
			bOK = reader.namedValue("id", id) ||
				loadFlow(reader, "Horz", horz) ||
				loadFlow(reader, "Vert", vert);
		}
		while (bOK && reader.comma());

		if (bOK)
		{
			List *pList = new List(id, theme);
			pList->setFlow(eDown, vert);
			pList->setFlow(eRight, horz);
			// Nested content cannot be loaded via JSON. Must come from a Set.
			// Don't have a JSON spec for Sets.
			pControl = pList;
		}
	}
	return bOK;
}
