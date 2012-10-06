#include "Flow.h"
#include "Button.h"
#include "Accessor.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

struct Edit;
struct Button;

struct PickFont : public Flow, private IAccessor<string_t>
{
	PickFont(identity_t id, Theme &theme, const TCHAR *prompt, IAccessor<Font>* access);
	virtual void setRect(const rect_t &rect);

private:
	void click(Button*, bool down);
	const string_t &getValue() const;
	bool setValue(const string_t &value);

	Edit* _edit;
	Button* _button;
	mutable string_t _text;
	IAccessor<Font>* _access;
	const TCHAR* _prompt;
};

};