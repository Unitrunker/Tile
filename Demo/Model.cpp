#include "stdafx.h"
#include "Model.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Model;

Channel *Site::newChannel(channel_t lcn, bool bAdd)
{
	Channel *p = NULL;
	Channels::iterator it = Channels.find(lcn);
	if (!it.end() && (*it)->_LCN == lcn)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Channel(this, lcn);
		Channels.insert(lcn, p);
	}

	if (bAdd) 
	{
	   if (!p->_hits)
		   time(&p->_first);
	   p->_hits++;
	   time(&p->_last);
	}
	return p;
}

Group *System::newGroup(address_t address, bool bAdd)
{
	Group *p = NULL;
	Groups::iterator it = Groups.find(address);
	if (!it.end() && (*it)->_address == address)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Group(this, address);
		Groups.insert(address, p);
	}

	if (bAdd) 
	{
	   if (!p->_hits)
		   time(&p->_first);
	   p->_hits++;
	   time(&p->_last);
	}
	return p;
}

User *System::newUser(address_t address, bool bAdd)
{
	User *p = NULL;
	Users::iterator it = Users.find(address);
	if (!it.end() && (*it)->_address == address)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new User(this, address);
		Users.insert(address, p);
	}

	if (bAdd) 
	{
	   if (!p->_hits)
		   time(&p->_first);
	   p->_hits++;
	   time(&p->_last);
	}
	return p;
}

Site *System::newSite(site_t site, bool bAdd)
{
	Site *p = NULL;
	Sites::iterator it = Sites.find(site);
	if (!it.end() && (*it)->_site == site)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Site(this, site);
		Sites.insert(site, p);
	}

	if (bAdd) 
	{
	   if (!p->_hits)
		   time(&p->_first);
	   p->_hits++;
	   time(&p->_last);
	}
	return p;
}

System *Model::Folder::newSystem(network_t network, bool bAdd)
{
	System *p = NULL;
	Systems::iterator it = Systems.find(network);
	if (!it.end() && (*it)->_network == network)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new System(this, network);
		Systems.insert(network, p);
	}

	return p;
}
