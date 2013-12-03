#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "../Tile/Tab.h"
#include "../Tile/Button.h"
#include "Model.h"
#include "Site.h"
#include "Group.h"
#include "User.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

// accessor adapter for string to long.
struct Trunk : public IAccessor<string_t>
{
	Trunk(IAccessor<Model::trunk_t>& wrap);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<Model::trunk_t>& _wrap;
	mutable string_t _text;
	Trunk & operator = (const Trunk &never);
};

struct SystemSet : public SetFollowT<Model::System>
{
	SystemSet(Theme&);
	bool getCaption(string_t &label) const;
	void setAdd(bool);
	bool getAdd() const;
private:
	ReflectSubtype<Model::System, Model::network_t, unsigned long> _identity;
	ReflectSubtype<Model::System, Model::network_t, Model::trunk_t> _trunk;
	Reflect<Model::System, time_t> _first;
	Reflect<Model::System, time_t> _last;
	Reflect<Model::System, string_t> Label;
	Trunk Trunk;
	UInteger Network;
	Time First;
	Time Last;
	bool _add;
	IControl* _primary[2];
};

struct SystemFrame : public Window
{
	SystemFrame(Factory& factory);
	SystemFrame(Factory& factory, Model::System *system);
	SystemFrame(Factory& factory, std::vector<Model::System *> &list);
	virtual ~SystemFrame();
	bool Create(RECT rect);
	SystemSet _set;
private:
	Pane *_top;
	Pane *_toolInfo;
	Pane *_toolSite;
	Pane *_toolGroup;
	Pane *_toolUser;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Factory& _factory;

	void inside();
	void remove();
	void clickEdit(Button *);

	void clickSitePlus(Button *);
	void clickSiteMinus(Button *);
	void clickSiteInspect(Button *);

	void clickGroupPlus(Button *);
	void clickGroupMinus(Button *);
	void clickGroupInspect(Button *);

	void clickUserPlus(Button *);
	void clickUserMinus(Button *);
	void clickUserInspect(Button *);

	void clickClear(Button *);

	void clickRefresh(Button *);
	void clickHome(Button *);
	void activateInfo(Tab *);
	void activateGroups(Tab *);
	void activateUsers(Tab *);
	void activateSites(Tab *);

	void activateGroupPopup(Grid *, size_t row, size_t col);
	void activateUserPopup(Grid *, size_t row, size_t col);
	void activateSitePopup(Grid *, size_t row, size_t col);

	Table<SiteSet, Model::Site> _sites;
	Table<UserSet, Model::User> _users;
	Table<GroupSet, Model::Group> _groups;
};

