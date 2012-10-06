#include "stdafx.h"
#include "List.h"
#include "Text.h"
#include "Edit.h"
#include "Check.h"
#include "Tree.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

// _Section describes a row in a List control.
// Expands and collapses items just like a Tree control.
struct _Section : public Tree
{
	_Section(identity_t id, Theme &theme);
	virtual ~_Section();

	// instance type
	virtual const char* getType() const;
	void setItems(Section *section);

protected:
	virtual void setFocus(IControl *pControl);

private:
	Section* _section;
};

_Section::_Section(identity_t id, Theme &theme) :
	Tree(id, theme), _section(NULL)
{
	_checked = true;
}

_Section::~_Section()
{
}

// instance type
const char* _Section::getType() const { return "Section"; }

void _Section::setItems(Section *section)
{
	_section = section;
	Theme& theme = _tile.getTheme();
	FlowDesc text = {0, 4096, 1, false};
	FlowDesc edit = {0, 4096, 3, false};
	FlowDesc line = {1, 1, 0, true};
	FlowDesc coil = {0, 4096, 0, false};
	Theme::Font textFont = {Theme::eText, theme.Text};
	Theme::Font arrowFont = {Theme::eArrow, theme.Arrow};

	setFlow(eDown, coil);
	setFlow(eRight, coil);

	Flow *pSection = new Flow(0, theme, eRight);
	Check *pCheck = new Check(0, theme, this);
	Text *pText = new Text(0, theme, textFont, eLeft, section->Caption);
	pText->setFlow(eRight, text);
	//pCheck->setGlyphs(_T("\x71"), _T("\x75"));
	//static Font dings(_T("Wingdings 3"), 18, 1);
	//static const TCHAR plus[] = {0xFEFF, 0x2295, 0};
	//static const TCHAR minus[] = {0xFEFF, 0x229D, 0};
	//static Font dings(_T("Lucida Sans Unicode"), 18, 0);
	//static Font dings(_T("Segoe UI Symbol"), 18, 0);
	//static Font dings(_T("Cambria"), 18, 0);
	pCheck->setGlyphs(theme.Collapse.c_str(), theme.Expand.c_str());
	pCheck->setAlign(eLeft|eRight);
	//pCheck->setFont(dings);
	pSection->Add(pCheck);
	pSection->Add(pText);
	pSection->setFlow(eDown, line);
	Flow::Add(pSection);
	_control = pCheck;
	size_t size = _section->Items.size();
	for (size_t j = 0; j < size; j++)
	{
		Property *item = _section->Items[j];
		Flow *pItem = new Flow(0, theme, eRight);
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
	Flow::setFocus(pControl);

	if ( getIndex() == 0 )
	{
		// tell property list control to update the descriptive text section.
		Flow *flow = getContainer();
		List *list = static_cast<List*>(flow);
		list->select(_section);
	}
	else
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
			Flow *flow = getContainer();
			List *list = static_cast<List*>(flow);
			list->select(_section->Items[i]);
		}
	}
}

//

List::List(identity_t id, Theme &theme) : 
	Flow(id, theme, eDown),
	_set(NULL), 
	_note(NULL)
{
	_scroll = true;
	_thick.local = true;
	_thick.thick = 0;
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
	if (size > 1)
	{
		_listControls.erase(_listControls.begin(), _listControls.begin() + size - 1);
		_note->_text = _T("Empty");
		_note->setChanged(true);
	}
}

void List::setItems(struct Set *set)
{
	_set = set;

	clear();

	if (_set)
	{
		Theme& theme = _tile.getTheme();
		Theme::Font textFont = {Theme::eText, theme.Text};
		FlowDesc text = {0, 4096, 1, false};
		FlowDesc edit = {0, 4096, 9, false};
		FlowDesc line = {1, 1, 0, true};
		for (size_t i = 0; i < _set->Sections.size(); i++)
		{
			_Section *pSection = new _Section(0, theme);
			pSection->setItems(_set->Sections[i]);
			Add(pSection, true);
		}
		_note = new Text(0, theme, textFont, eLeft|eUp, _T("Some interesting words go here."));
		_note->setFlow(eDown, text);
		Add(_note);
	}
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
