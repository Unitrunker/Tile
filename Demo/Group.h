#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "../Tile/Button.h"
#include "../Tile/Tab.h"
#include "Model.h"
#include "Address.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once 

struct GroupSet : public SetFollowT<Model::Group>, public IColorAccess
{
	GroupSet(Theme&);
	bool getCaption(string_t &label) const;
	void setAdd(bool);
	bool getAdd() const;
private:
	Reflect<Model::Group, Model::address_t> _address;
	Reflect<Model::Group, time_t> _first;
	Reflect<Model::Group, time_t> _last;
	Reflect<Model::Group, color_t> _color;
	Reflect<Model::Group, Model::rank_t> _rank;
	Reflect<Model::Group, Model::hit_t> _hits;
	Reflect<Model::Group, Model::alarm_t> _alarm;
	Adapter<long, Model::rank_t> _adaptRank;
	Adapter<long, Model::hit_t> _adaptHits;
	Adapter<long, Model::alarm_t> _adaptAlarm;

	// primary key
	IControl *_primary;
	bool _add;
	Address Address;
	Reflect<Model::Group, string_t> Label;
	Time First;
	Time Last;
	Integer Rank;
	Integer Hits;
	// retrieve color.
	virtual const color_t getValue(const Theme &theme, bool focus) const;
};

struct GroupFrame : public Window
{
	GroupFrame(Factory& factory, Model::System *system);
	GroupFrame(Factory& factory, Model::Group *group);
	GroupFrame(Factory& factory, std::vector<Model::Group *> &);
	virtual ~GroupFrame();
	bool Create(RECT rect);

	GroupSet _set;
private:
	Pane *_top;
	Pane *_tools;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Factory& _factory;

	void clickHome(Button *);
	void clickRefresh(Button *);
	void clickCommit(Button *);
	void activateInfo(Tab *);
	void activateHistory(Tab *);
	void inside();
	void remove();
};

