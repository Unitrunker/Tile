#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "../Tile/Tab.h"
#include "Model.h"
#include "Address.h"

/*
Copyright © 2012 Rick Parrish
*/

struct GroupSet : public SetT<Model::Group>
{
	GroupSet(Theme&);
private:
	MemberAccessPtr<Model::Group, Model::address_t> _address;
	MemberAccessPtr<Model::Group, time_t> _first;
	MemberAccessPtr<Model::Group, time_t> _last;
	Address Address;
	Time First;
	Time Last;
};

struct GroupFrame : public Window
{
	GroupFrame(Theme &theme, Model::Group *group);
private:
	Pane *_top;
	Tab *_tools;
	Tab *_tabset;
	Grid *_grid;
	List *_list;
	Model::Group *_group;
	GroupSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateHistory(Button *, bool up);
};

