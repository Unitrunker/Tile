#include "IControl.h"
#include "Tile.h"
#include "Theme.h"
#include "IWindow.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Control : public IControl
{
	Control(identity_t id, Theme &theme, const Theme::Font &desc);
	Control(identity_t id, Theme &theme);
	virtual ~Control();
	// get/set accessors for changed flag.
	virtual bool getChanged() const;
	virtual void setChanged(bool bChanged);
	// set callback for change notification.
	virtual void watch(IRedraw*);

	// IControl
	// hover
	virtual void setHover(bool);
	// focus
	virtual bool getFocus() const;
	virtual void setFocus(bool);
	// readonly
	virtual bool getReadOnly() const;
	virtual void setReadOnly(bool);
	// navigation
	virtual bool onNear(orient_t);
	virtual bool onFar(orient_t);
	virtual bool hasControls() const { return true; }
	// desktop
	virtual void setDesktop(IWindow *);

	// ITile implementation
	virtual identity_t identity() const;
	// get/set accessors for layout mimimums
	virtual void getMin(orient_t flow, meter_t &min);
	virtual void setMin(orient_t flow, meter_t min);
	// get/set accessors for layout maximums
	virtual void getMax(orient_t flow, meter_t &max);
	virtual void setMax(orient_t flow, meter_t max);
	// get/set accessors for layout weights
	virtual void getWeight(orient_t flow, meter_t &weight);
	virtual void setWeight(orient_t flow, meter_t weight);
	// get/set accessors for layout descriptors
	virtual void getFlow(orient_t flow, Flow &desc);
	virtual void setFlow(orient_t flow, const Flow &desc);
	// get/set accessor for bounding rectangle.
	virtual void getRect(rect_t &rect) const;
	virtual void setRect(const rect_t &rect);
	// get/set accessor for scroll box - width, height, and (x,y) offset
	// where the rect above exposes a portion of the scroll box.
	virtual void getScrollBox(rect_t &box) const;
	virtual void setScrollBox(const rect_t &box);
	virtual void getScrollArea(area_t &rect) const;
	virtual void setScrollArea(const area_t &area);
	virtual void getScrollPoint(point_t &rect) const;
	virtual void setScrollPoint(const point_t &area);
    // true if point lies in this tile.
	virtual bool contains(point_t pt);
	// the tile's containing flow object
	virtual Pane *getContainer() const;
	virtual void setContainer(Pane *);
	// sink all mouse click/move events.
	virtual bool dispatch(MouseEvent &action);
	// get font selector
	const Theme::Font &getFont() const;
	void setFont(const Theme::Font &font);

protected:
	// display logic.
	Tile _tile;
	// our parent window.
	IWindow* _pDesktop;
	/// true if this control has focus
	bool _focus;
	/// true if mouse hovering over this control.
	bool _hover;
	/// true if this control is display-only
	bool _readonly;
};

};