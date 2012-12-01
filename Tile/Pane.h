#include <vector>
#include <map>
#include "Tile.h"
#include "IControl.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

/*
Manages a collection of ITile objects that "flow" horizontally or vertically within
the confines of a bounding rectangle. In the flow direction, each layout object has a
minimum length, maximum length, and a weight used to calculate how much surplus space
can be absorbed by the object.
*/
struct Pane : public IControl, public IRedraw
{
	// returns opposite direction
	static orient_t opposite(orient_t flow);
	// returns true for vertical flows
	static bool vertical(orient_t flow);
	// returns true for horizontal flows
	static bool horizontal(orient_t flow);

	Pane(identity_t id, Theme &theme, orient_t Pane = eRight);
	Pane(identity_t id, Theme &theme, Theme::Font& desc, orient_t Pane = eRight);
	virtual ~Pane();

	void clear();

	// ITile implementation
	virtual identity_t identity() const;
	// instance type
	virtual const char* getType() const;
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
	virtual void getScrollPoint(point_t &pt) const;
	virtual void setScrollPoint(const point_t &pt);
	// returns true if point lies within layout.
	virtual bool contains(point_t pt) const;
	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, Pane *&pPane);

	// draw this pane (and the drawable artifacts contained therein).
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// get/set accessors for changed flag.
	virtual bool getChanged() const;
	virtual void setChanged(bool bChanged);
	// set callback for change notification.
	virtual void watch(IRedraw*);
	// IControl
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);
	// focus
	virtual bool getFocus() const;
	virtual void setFocus(bool);
	// readonly
	virtual bool getReadOnly() const;
	virtual void setReadOnly(bool);
	// enable
	virtual bool getEnable() const;
	virtual void setEnable(bool);
	// force an update of any edits in-progress.
	virtual void apply();
	// hover
	virtual void setHover(bool);
	// navigation
	virtual bool onNear(orient_t);
	virtual bool onFar(orient_t);
	virtual bool hasControls() const;
	// desktop
	virtual void setDesktop(IWindow*);
	// the tile's containing flow object
	virtual Pane *getContainer() const;
	virtual void setContainer(Pane *);

	bool isRoot() const;
	// pixel padding between child tiles.
	void setSpace(meter_t space);
	// pixel padding along border.
	void setBorder(meter_t border);

	// save as JSON
	bool save(const TCHAR *path);

	// add layout item to the collection.
 	size_t Add(ITile* pTile, meter_t min, meter_t max, meter_t weight, bool fontScale = false);
	// add layout item to the collection.
	size_t Add(ITile* pTile);
	// add layout item to the collection.
	size_t Add(IControl* pControl, meter_t min, meter_t max, meter_t weight, bool fontScale = false);
	// add layout item to the collection.
	size_t Add(IControl* pControl);
	// add layout item to the collection.
	size_t Add(Pane* pPane, meter_t min, meter_t max, meter_t weight, bool fontScale = false);
	// add layout item to the collection.
	size_t Add(Pane* pPane, bool local = false);
	// recompute items layout.
	void reflow();

	orient_t getFlow() const;

	static const char *getName(orient_t flow);

	// IRedraw - draw this flow's tiles plus any intervening space.
	virtual void Redraw(ITile *pDraw);

	// child control index property getter/setters.
	void setIndex(size_t);
	size_t getIndex() const;
	// for persistence.
	static const char* type();

	// Binding mechanism. retrieves a control by type and ID. Both must match.
	bool find(identity_t id, const char *type, IControl *&pControl);

	// get number of child controls.
	size_t getControlCount() const;
	// get child control by index.
	IControl* getControl(size_t);

	// Focus chaining: child controls call this to take focus.
	virtual void setFocus(IControl *pControl);

	// experimental.
	void Shown(ITile *pTile);
	void getTileBox(ITile *pTile, rect_t &box);
	void balance(rect_t tile);
	// rect is visible area
	// box is scrollable area
	// border is excluded
	void layout(rect_t rect, rect_t box);

	const Theme::Font &getFont() const;
	void setFont(const Theme::Font &font);
	void setLineColor(const Theme::Color &color);
	void setLineThick(const Theme::Thick &thick);

protected:
	bool onTab(bool bReverse);
	void getInsideMin(orient_t flow, meter_t &min);
	virtual void onIndexChanged(size_t index);

	Tile _tile;
	// inter-cell spacing.
	Theme::Thick _space;
	// inter-cell color
	Theme::Color _lines;
	// border thickness
	Theme::Thick _thick;
	// border color
	Theme::Color _border;
	// tiles to be drawn
	std::vector<ITile*> _listTiles;
	// controls for mouse, key, hover, and focus.
	std::vector<IControl*> _listControls;
	// top-level map for binding.
	std::map<identity_t, IControl*> _map;
	// direction of layout flow.
	orient_t _flow;
	// index to currently (or most recently) active control.
	size_t _index;
	// index to be shared by child flow controls.
	size_t _shared;
	// index to child control with hover.
	size_t _hover;
	// read only
	bool _readOnly;
	// enable
	bool _enable;
	// true if this flow uses own _index instead of parent's _shared index.
	// sibling flows may use a shared index to allow cursor navigation
	// across rows and columns. Moving across columns stay in same row. 
	// Moving across rows but stay in same column. Sneaky, eh?
	bool _local;
	// allow scrolling (set to false to defer to container)
	bool _scroll;
	// the owning window.
	IWindow* _pDesktop;
	// where to send redraw notices (usually also the owning window).
	IRedraw* _pNotify;
};

};