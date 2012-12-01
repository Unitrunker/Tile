#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "Model.h"
#include "Address.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

struct UserSet : public SetFollowT<Model::User>, public IColorAccess
{
	UserSet(Theme&);
	bool getCaption(string_t &label) const;
	void setAdd(bool);
	bool getAdd() const;
private:
	Reflect<Model::User, Model::address_t> _address;
	Reflect<Model::User, time_t> _first;
	Reflect<Model::User, time_t> _last;
	Reflect<Model::User, color_t> _color;
	Reflect<Model::User, Model::rank_t> _rank;
	Reflect<Model::User, Model::hit_t> _hits;
	Reflect<Model::User, Model::alarm_t> _alarm;
	Adapter<long, Model::rank_t> _adaptRank;
	Adapter<long, Model::hit_t> _adaptHits;
	Adapter<long, Model::alarm_t> _adaptAlarm;

	IControl* _primary;
	bool _add;
	Address Address;
	Reflect<Model::User, string_t> Label;
	Time First;
	Time Last;
	Integer Rank;
	Integer Hits;
	// retrieve color.
	virtual const color_t getValue(const Theme &theme, bool focus) const;
};

struct UserFrame : public Window
{
	UserFrame(Factory& factory, Model::System *system);
	UserFrame(Factory& factory, Model::User *user);
	UserFrame(Factory& factory, std::vector<Model::User *> &list);
	virtual ~UserFrame();
	bool Create(RECT rect);

	UserSet _set;
private:
	Pane *_top;
	Pane *_tools;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Factory& _factory;

	void inside();
	void remove();
	void clickCommit(Button *);
	void clickHome(Button *);
	void clickRefresh(Button *);
	void activateInfo(Tab*);
	void activateHistory(Tab*);
};

