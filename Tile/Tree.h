#include "Accessor.h"
#include "Pane.h"
#include "Check.h"
#include "Text.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

// Tree describes a collapsible control.
// Hint: Tree controls can be nested.
struct Tree : public Pane, public IAccessor<bool>
{
	Tree(identity_t id, Theme &theme, const Theme::Font &font, const TCHAR *text = _T(""));
	virtual ~Tree();

	// accessor for expand / collapse.
	virtual const bool& getValue() const;
	virtual bool setValue(const bool &value);
	// add layout item to the collection.
	size_t Add(IControl* pControl);
	// instance type
	virtual const char* getType() const;
	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pDraw);
	static const char* type();

protected:
	void hideControls();
	std::vector<IControl*> _listHidden;
	Pane _header;
	Check _control;
	Text _label;
	bool _checked;
};

};