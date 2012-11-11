#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "../Tile/Tab.h"
#include "Model.h"
#include "Site.h"
#include "Group.h"
#include "User.h"

/*
Copyright © 2012 Rick Parrish
*/

struct SystemSet : public SetT<Model::System>
{
	SystemSet(Theme&);

	virtual bool setValue(Model::System* const &value)
	{
		_network.setValue(&value->_network);
		return SetT<Model::System>::setValue(value);
	}
private:
	Tiles::Value<Model::network_t *> _network;
	MemberAccessPtr<Model::network_t, unsigned long> _identity;
	MemberAccessPtr<Model::System, time_t> _first;
	MemberAccessPtr<Model::System, time_t> _last;
	UInteger Network;
	Time First;
	Time Last;
};

struct SystemFrame : public Window
{
	SystemFrame(Theme &theme, Model::System *system);
private:
	Pane *_top;
	Tab *_tools;
	Tab *_tabset;
	Grid *_grid;
	List *_list;
	Model::System *_system;
	SystemSet _set;

	void activateInfo(Button *, bool up);
	void activateGroups(Button *, bool up);
	void activateUsers(Button *, bool up);
	void activateSites(Button *, bool up);

	void activateGroupPopup(Grid *, size_t row, size_t col);
	void activateUserPopup(Grid *, size_t row, size_t col);
	void activateSitePopup(Grid *, size_t row, size_t col);

	Table<SiteSet, Model::Site> _sites;
	Table<UserSet, Model::User> _users;
	Table<GroupSet, Model::Group> _groups;
};

