#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
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

struct UserSet : public SetT<Model::User>
{
	UserSet(Theme&);
private:
	MemberAccessPtr<Model::User, Model::address_t> _address;
	MemberAccessPtr<Model::User, time_t> _first;
	MemberAccessPtr<Model::User, time_t> _last;
	Address Address;
	Time First;
	Time Last;
};

struct UserFrame : public Window
{
	UserFrame(Factory& factory, Model::User *user);
	virtual ~UserFrame();
	bool Create(RECT rect);
private:
	Pane *_top;
	Tab *_tools;
	Tab *_tabset;
	Grid *_grid;
	List *_list;
	Model::User *_user;
	UserSet _set;
	Factory& _factory;

	void clickHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateHistory(Button *, bool up);
};

