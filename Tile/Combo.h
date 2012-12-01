#include "Arrow.h"
#include "Text.h"
#include "Pane.h"
#include "Theme.h"
#include "IWindow.h"
#include "Accessor.h"
#include "Colorful.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Combo : public Pane, public Colorful
{
	struct Item
	{
		const TCHAR *_label;
		long _value;
	};

	Combo(identity_t id, Theme &theme, Theme::Font& font, std::vector<Combo::Item> &list, IAccessor<long>* access);
	Combo(identity_t id, Theme &theme, Theme::Font& font, Combo::Item list[], size_t size, IAccessor<long>* access);
	Combo(identity_t id, Theme &theme, Theme::Font& font, std::vector<Combo::Item> &list, IAccessor<string_t>* access);
	virtual ~Combo();

	// get/set accessors for changed flag.
	using Pane::getChanged;
	using Pane::setChanged;
	// set callback for change notification.
	using Pane::watch;
	// IControl
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);
	// focus
	virtual bool getFocus() const;
	virtual void setFocus(bool);
	// readonly
	using Pane::getReadOnly;
	using Pane::setReadOnly;
	// hover
	virtual void setHover(bool hover);
	// navigation
	virtual bool onNear(orient_t);
	virtual bool onFar(orient_t);
	virtual bool hasControls() const { return true; }

	void getLabel(string_t &value);
	bool setLabel(const string_t &value);

	void getValue(long &value);
	bool setValue(long value);
	// ITile implementation
	using Pane::identity;
	// instance type
	virtual const char* getType() const;

	// get/set accessors for layout mimimums
	virtual void getMin(orient_t flow, meter_t &min);
	using Pane::setMin;
	// get/set accessors for layout maximums
	virtual void getMax(orient_t flow, meter_t &max);
	using Pane::setMax;
	// get/set accessors for layout weights
	virtual void getWeight(orient_t flow, meter_t &weight);
	using Pane::setWeight;
	// get/set accessors for layout descriptors
	using Pane::getFlow;
	using Pane::setFlow;
	// get/set accessor for bounding rectangle.
	virtual void setRect(const rect_t &rect);
	using Pane::getRect;

	using Pane::contains;
	using Pane::setDesktop;
	using Pane::getContainer;
	using Pane::setContainer;
	//using Pane::Draw;
	bool Draw(ICanvas *canvas, bool bFocus);
	// serialize
	virtual bool save(JSON::Writer &writer);

	void closePopup();

	static const char* type();
private:
	void synchronize();
	void setIndex(size_t index);
	Arrow _drop;
	Text _text;
	bool _focus;
	size_t _index;
	std::vector<Item> _list;
	IAccessor<long> *_accessInt;
	IAccessor<string_t> *_accessText;
	Theme _theme;
	IWindow* _popup;
	Pane *_popPane;

	bool _reflow;
};

};