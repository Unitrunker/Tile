#include "ITile.h"
#include "Theme.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Tile : public ITile
{
	Tile(identity_t id, Theme& theme);
	Tile(identity_t id, Theme& theme, const Theme::Font& desc);
	Tile(const Tile &);
	virtual ~Tile() { };

	// identity
	virtual identity_t identity() const;
	// instance type
	virtual const char* getType() const;

	virtual void getMin(orient_t flow, meter_t &min) const;
	virtual void setMin(orient_t flow, meter_t min);

	virtual void getMax(orient_t flow, meter_t &max) const;
	virtual void setMax(orient_t flow, meter_t max);

	virtual void getWeight(orient_t flow, meter_t &weight) const;
	virtual void setWeight(orient_t flow, meter_t weight);

	virtual void getFlow(orient_t flow, Flow &desc) const;
	virtual void setFlow(orient_t flow, const Flow &desc);

	// get/set accessor for bounding rectangle.
	virtual void getRect(rect_t &rect) const;
	virtual void setRect(const rect_t &rect);
	// get/set accessor for scroll box - width, height, and (x,y) offset
	// where the rect above exposes a portion of the scroll box.
	virtual void getScrollBox(rect_t &box) const;
	virtual void setScrollBox(const rect_t &box);
	virtual void getScrollArea(area_t &area) const;
	virtual void setScrollArea(const area_t &area);
	virtual void getScrollPoint(point_t &pt) const;
	virtual void setScrollPoint(const point_t &pt);
    // true if point lies in this tile.
	virtual bool contains(point_t pt) const;
	// serialize
	virtual bool save(JSON::Writer &);
	// the tile's containing flow object
	virtual Pane *getContainer() const;
	virtual void setContainer(Pane *);
	// get color from underlying theme.
	color_t getColor(const Theme::Color &color) const;
	// get grid thickness
	meter_t getThick(const Theme::Thick &thick) const;
	// get font
	const Font& getFont(const Theme::Font &font) const;
	// get theme object
	Theme& getTheme();
	const Theme& Tile::getTheme() const;
	// get font selector
	const Theme::Font &getFont() const;
	void setFont(const Theme::Font &font);

	// drawable
	// get/set accessor: true if this has changed since last drawn.
	// used to drive internal redrawing.
	virtual bool getChanged() const;
	virtual void setChanged(bool);
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);

	// set callback for change notification.
	virtual void watch(IRedraw*);

	const rect_t &rect() const;
	const rect_t &scrollBox() const;

	ITile* _pSelf;

protected:
	identity_t _id;
	rect_t _rect;
	rect_t _scrollBox;
	Flow _vert;
	Flow _horz;
	Theme& _theme;
	Theme::Font _font;
	IRedraw* _pNotify;
	bool _bChanged;
	Pane *_pPane;
private:
	Tile & operator = (const Tile &never);
};

};