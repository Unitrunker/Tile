#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "Model.h"
#include "Channel.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

// accessor adapter for string to site_t.
struct Site : public IAccessor<string_t>
{
	Site(IAccessor<Model::site_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<Model::site_t>& _wrap;
	mutable string_t _text;
	int _base;
	Site& operator = (const Site &never);
};

struct SiteSet : public SetFollowT<Model::Site>
{
	SiteSet(Theme&);
	bool getCaption(string_t &label) const;
	void setAdd(bool);
	bool getAdd() const;
private:
	Reflect<Model::Site, Model::site_t> _site;
	Reflect<Model::Site, time_t> _first;
	Reflect<Model::Site, time_t> _last;
	Reflect<Model::Site, string_t> Label;
	IControl* _primary;
	bool _add;
	Site Site;
	Time First;
	Time Last;
};

struct SiteFrame : public Window
{
	SiteFrame(Factory& factory, Model::System *system);
	SiteFrame(Factory& factory, Model::Site *site);
	SiteFrame(Factory& factory, std::vector<Model::Site *> &list);
	virtual ~SiteFrame();
	bool Create(RECT rect);

	SiteSet _set;
private:
	Pane *_top;
	Pane *_tools;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Factory& _factory;

	void inside();
	void remove();
	void clickHome(Button *);
	void clickEdit(Button *);
	void clickPlus(Button *);
	void clickMinus(Button *);
	void clickInspect(Button *);
	void clickRefresh(Button *);
	void clickClear(Button *);
	void clickCommit(Button *);
	void activateInfo(Tab*);
	void activateChannels(Tab*);
	void activateHistory(Tab*);

	void activateChannelPopup(Grid *, size_t row, size_t col);

	Table<ChannelSet, Model::Channel> _channels;
};

