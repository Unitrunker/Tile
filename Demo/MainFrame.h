#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "Model.h"

/*
Copyright © 2012 Rick Parrish
*/

// accessor adapter for string to channel_t.
struct LCN : public IAccessor<string_t>
{
	LCN(IAccessor<channel_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<channel_t>& _wrap;
	mutable string_t _text;
	int _base;
	LCN& operator = (const LCN &never);
};

// accessor adapter for string to site_t.
struct Site : public IAccessor<string_t>
{
	Site(IAccessor<site_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<site_t>& _wrap;
	mutable string_t _text;
	int _base;
	Site& operator = (const Site &never);
};

// accessor adapter for string to address_t.
struct Address : public IAccessor<string_t>
{
	Address(IAccessor<address_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<address_t>& _wrap;
	mutable string_t _text;
	int _base;
	Address& operator = (const Address &never);
};

struct SystemSet : public SetT<Model::System>
{
	SystemSet(Theme&);
private:
	MemberAccessPtr<Model::System, network_t> _network;
	MemberAccessPtr<Model::System, time_t> _first;
	MemberAccessPtr<Model::System, time_t> _last;
	UInteger Network;
	Time First;
	Time Last;
};

struct MainFrame : public Frame
{
	MainFrame(Theme &theme);
private:
	void clickety(Grid *, size_t row, size_t col);
	Table<SystemSet, Model::System> _table;
	Model::Folder _folder;
};

struct ChannelSet : public SetT<Model::Channel>
{
	ChannelSet(Theme&);
private:
	MemberAccessPtr<Model::Channel, channel_t> _lcn;
	MemberAccessPtr<Model::Channel, frequency_t> _frequency;
	MemberAccessPtr<Model::Channel, time_t> _first;
	MemberAccessPtr<Model::Channel, time_t> _last;
	LCN LCN;
	UInteger Frequency;
	Time First;
	Time Last;
};

struct UserSet : public SetT<Model::User>
{
	UserSet(Theme&);
private:
	MemberAccessPtr<Model::User, network_t> _address;
	MemberAccessPtr<Model::User, time_t> _first;
	MemberAccessPtr<Model::User, time_t> _last;
	Address Address;
	Time First;
	Time Last;
};

struct GroupSet : public SetT<Model::Group>
{
	GroupSet(Theme&);
private:
	MemberAccessPtr<Model::Group, network_t> _address;
	MemberAccessPtr<Model::Group, time_t> _first;
	MemberAccessPtr<Model::Group, time_t> _last;
	Address Address;
	Time First;
	Time Last;
};

struct SiteSet : public SetT<Model::Site>
{
	SiteSet(Theme&);
private:
	MemberAccessPtr<Model::Site, site_t> _site;
	MemberAccessPtr<Model::Site, time_t> _first;
	MemberAccessPtr<Model::Site, time_t> _last;
	Site Site;
	Time First;
	Time Last;
};

struct SystemFrame : public Window
{
	SystemFrame(Theme &theme, Model::System *system);
private:
	Pane *_top;
	Pane *_tabset;
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

struct SiteFrame : public Window
{
	SiteFrame(Theme &theme, Model::Site *site);
private:
	Pane *_top;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Model::Site *_site;
	SiteSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateChannels(Button *, bool up);

	void activateChannelPopup(Grid *, size_t row, size_t col);

	Table<ChannelSet, Model::Channel> _channels;
};

struct ChannelFrame : public Window
{
	ChannelFrame(Theme &theme, Model::Channel *channel);
private:
	Pane *_top;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Model::Channel *_channel;
	ChannelSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateHistory(Button *, bool up);
};

struct GroupFrame : public Window
{
	GroupFrame(Theme &theme, Model::Group *group);
private:
	Pane *_top;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Model::Group *_group;
	GroupSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateHistory(Button *, bool up);
};

struct UserFrame : public Window
{
	UserFrame(Theme &theme, Model::User *user);
private:
	Pane *_top;
	Pane *_tabset;
	Grid *_grid;
	List *_list;
	Model::User *_user;
	UserSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateHistory(Button *, bool up);
};

