#include "../Tile/AVL.h"
#include <time.h>
#include "Types.h"
#include "../Tile/Types.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

namespace Model
{

// forwards
struct Site;
struct Channel;
struct Group;
struct User;
struct System;
struct Folder;

///
/// Channel - repeater transmit frequency for voice and data.
///
struct Channel : Follow<Channel*>
{
	Site *_site;
	channel_t _LCN;
	frequency_t _hz;
	hit_t _hits;
	time_t _first, _last;
	Tiles::string_t _label;
	service_t _service;

	Channel(Site *site, channel_t lcn) : _site(site), _LCN(lcn), _hits(0), _first(0), _last(0), _hz(0), _service(0) { };

	static bool less(const channel_t &left, const channel_t &right) { return left < right; }
	static channel_t byLCN(Channel * const p) { return p->_LCN; }
	static hit_t byHits(Channel * const p) { return p->_hits; }
	static time_t byFirst(Channel * const p) { return p->_first; }
	static time_t byLast(Channel * const p) { return p->_last; }
	static Tiles::string_t &byLabel(Channel * const p) { return p->_label; }
};

typedef AVL<channel_t, Channel *, Channel *, Channel::less, Channel::byLCN> Channels;

///
/// Repeater site - manages list of channels. Only the TX frequency of each repeater pair is tracked here.
///
struct Site : Follow<Site*>
{
	System *_system;
	site_t _site;
	Channels Channels;
	hit_t _hits;
	time_t _first, _last;
	Tiles::string_t _label;

	Site(System *system, site_t site) : _system(system), _site(site), _hits(0), _first(0), _last(0) { };

	// factory method.
	virtual Channel *newChannel(channel_t lcn, bool bAdd);

	static bool less(const site_t &left, const site_t &right) { return left < right; }
	static site_t bySite(Site * const p) { return p->_site; }
	static hit_t byHits(Site * const p) { return p->_hits; }
	static time_t byFirst(Site * const p) { return p->_first; }
	static time_t byLast(Site * const p) { return p->_last; }
	static Tiles::string_t &byLabel(Site * const p) { return p->_label; }
};

///
/// Talkgroup
///
struct Group : Follow<Group*>
{
	System *_system;
	address_t _address;
	hit_t _hits;
	time_t _first, _last;
	Tiles::string_t _label;

	Group(System *system, address_t address) : _system(system), _address(address), _hits(0), _first(0), _last(0) { };

	static bool less(const address_t &left, const address_t &right) { return left < right; }
	static address_t byAddress(Group * const p) { return p->_address; }
	static hit_t byHits(Group * const p) { return p->_hits; }
	static time_t byFirst(Group * const p) { return p->_first; }
	static time_t byLast(Group * const p) { return p->_last; }
	static Tiles::string_t &byLabel(Group * const p) { return p->_label; }
};

///
/// Radio user
///
struct User : Follow<User*>
{
	System *_system;
	address_t _address;
	hit_t _hits;
	time_t _first, _last;
	Tiles::string_t _label;

	User(System *system, address_t address) : _system(system), _address(address), _hits(0), _first(0), _last(0) { };

	static bool less(const address_t &left, const address_t &right) { return left < right; }
	static address_t byAddress(User * const p) { return p->_address; }
	static hit_t byHits(User * const p) { return p->_hits; }
	static time_t byFirst(User * const p) { return p->_first; }
	static time_t byLast(User * const p) { return p->_last; }
	static Tiles::string_t &byLabel(User * const p) { return p->_label; }
};

typedef AVL<address_t, User *, User *, User::less, User::byAddress> Users;
typedef AVL<address_t, Group *, Group *, Group::less, Group::byAddress> Groups;
typedef AVL<site_t, Site *, Site *, Site::less, Site::bySite> Sites;

///
/// System - manages a collection of talkgroups, radio IDs, and repeater sites.
///
struct System : Follow<System*>
{
	Folder *_folder;
	trunk_t _type;
	network_t _network;
	time_t _first, _last;
	Tiles::string_t _label;

	Users Users;
	Groups Groups;
	Sites Sites;

	System(Folder *folder, network_t network) : _folder(folder), _network(network), _first(0), _last(0) { };

	// factory method.
	virtual Site *newSite(site_t site, bool bAdd);
	// factory method.
	virtual User *newUser(address_t address, bool bAdd);
	// factory method.
	virtual Group *newGroup(address_t address, bool bAdd);

	static network_t byNetwork(System * const p) { return p->_network; }
	static time_t byFirst(System * const p) { return p->_first; }
	static time_t byLast(System * const p) { return p->_last; }
	static Tiles::string_t &byLabel(System * const p) { return p->_label; }
};

typedef AVL<network_t, System *, System *, network_t::less, System::byNetwork> Systems;

///
///
///
struct Folder : Follow<Folder*>
{
	Systems Systems;

	// factory method.
	virtual System *newSystem(network_t network, bool bAdd);
};

};