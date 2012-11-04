#include "stdafx.h"
#include "Model.h"

/*
Copyright © 2012 Rick Parrish
*/

Model::Channel *Model::Site::newChannel(channel_t lcn, bool bAdd)
{
	Model::Channel *p = NULL;
	Model::Channels::iterator it = Channels.find(lcn);
	if (!it.end() && (*it)->_LCN == lcn)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Model::Channel(this, lcn);
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

Model::Group *Model::System::newGroup(address_t address, bool bAdd)
{
	Model::Group *p = NULL;
	Model::Groups::iterator it = Groups.find(address);
	if (!it.end() && (*it)->_address == address)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Model::Group(this, address);
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

Model::User *Model::System::newUser(address_t address, bool bAdd)
{
	Model::User *p = NULL;
	Model::Users::iterator it = Users.find(address);
	if (!it.end() && (*it)->_address == address)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Model::User(this, address);
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

Model::Site *Model::System::newSite(site_t site, bool bAdd)
{
	Model::Site *p = NULL;
	Model::Sites::iterator it = Sites.find(site);
	if (!it.end() && (*it)->_site == site)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Model::Site(this, site);
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

Model::System *Model::Folder::newSystem(network_t network, bool bAdd)
{
	Model::System *p = NULL;
	Model::Systems::iterator it = Systems.find(network);
	if (!it.end() && (*it)->_network == network)
	{
		p = (*it);
	}
	else if (bAdd)
	{
		p = new Model::System(this, network);
		Systems.insert(network, p);
	}

	return p;
}
