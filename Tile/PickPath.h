#include "Flow.h"
#include "Button.h"
#include "Edit.h"
#include "Accessor.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

struct PickPath : public Flow
{
	PickPath(identity_t id, Theme &theme, const TCHAR *prompt, IAccessor<string_t>* access);
	~PickPath();

	virtual void setRect(const rect_t &rect);

private:
	void click(Button*, bool down);

	IAccessor<string_t>* _access;
	const TCHAR* _prompt;
	Edit* _edit;
	Button* _button;
};

};