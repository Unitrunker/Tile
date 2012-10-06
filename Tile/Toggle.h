#include "Control.h"
#include "Theme.h"
#include "IWindow.h"
#include "Accessor.h"
#include <vector>

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Toggle : public Control
{
	struct Item
	{
		TCHAR *_label;
		long _value;
	};

	Toggle(identity_t id, Theme &theme, Theme::Font& desc, std::vector<Item> & list, long& value);
	Toggle(identity_t id, Theme &theme, Theme::Font& desc, std::vector<Item> & list, IAccessor<long>* pAccess);
	virtual ~Toggle() { };
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);

	// IControl
	// hover
	virtual void setHover(bool);
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);

	// instance type
	virtual const char* getType() const;
	// serialize
	virtual bool save(JSON::Writer &writer);
	static const char* type();
	void setAccessor(IAccessor<long> *);
private:
	void step();

	std::vector<Item>& _list;
	IAccessor<long>* _access;
	Reference<long> _inside;
	size_t _cursor;
    /// <param name="orient">orientation</param>
	align_t _align;
	long _ignore;
	Theme::Color _fore[2], _back[2];
};

};