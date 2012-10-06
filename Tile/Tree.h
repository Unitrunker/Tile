#include "Accessor.h"
#include "Flow.h"
#include "Check.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

// Tree describes a collapsible control.
// Hint: Tree controls can be nested.
struct Tree : public Flow, public IAccessor<bool>
{
	Tree(identity_t id, Theme &theme);
	virtual ~Tree();

	virtual const bool& getValue() const;
	virtual bool setValue(const bool &value);
	// add layout item to the collection.
	size_t Add(IControl* pControl);
	// instance type
	virtual const char* getType() const;

protected:
	void hideControls();
	std::vector<IControl*> _listHidden;
	Check* _control;
	bool _checked;
};

};