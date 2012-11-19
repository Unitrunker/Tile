#include "stdafx.h"
#include "Factory.h"
#include "Channel.h"
#include "Site.h"
#include "Group.h"
#include "User.h"
#include "Folder.h"
#include "Receiver.h"

Factory::Factory() : _frame(NULL)
{
	time_t now = 0;
	time(&now);

	// generate fake data.
	size_t size = _folder.Systems.size();
	_ASSERT( 0 == size );
	srand( (unsigned)time(NULL) );
	for (size_t i = 0; i < 10; i++)
	{
		Model::network_t network = {0};
		network._type = Model::P25;
		network._network = 100 - i;
		Model::System *system = _folder.newSystem(network, true);
		size = _folder.Systems.size();
		_ASSERT( size == (i + 1) );
		system->_first = system->_last = now;
		for (size_t j = 0; j < 5; j++)
		{
			Model::address_t address = rand();
			Model::Group *group = system->newGroup(address, true);
			group->_first = group->_first = now;
			address = rand();
			Model::User *user = system->newUser(address, true);
			user->_first = user->_first = now;
			Model::site_t peer = (Model::site_t)rand();
			Model::Site *site = system->newSite(peer, true);
			site->_first = site->_first = now;
			for (size_t k = 0; k < 5; k++)
			{
				Model::channel_t lcn = (Model::channel_t)rand();
				Model::Channel *channel = site->newChannel(lcn, true);
				channel->_first = channel->_first = now;
			}
		}
	}
}

Factory::~Factory()
{
	while (_mapSystems.size())
		_mapSystems.begin()->second->DestroyWindow();
	while (_mapGroups.size())
		_mapGroups.begin()->second->DestroyWindow();
	while (_mapUsers.size())
		_mapUsers.begin()->second->DestroyWindow();
	while (_mapSites.size())
		_mapSites.begin()->second->DestroyWindow();
	while (_mapChannels.size())
		_mapChannels.begin()->second->DestroyWindow();
	while (_mapReceivers.size())
		_mapReceivers.begin()->second->DestroyWindow();
	if (!_frame != NULL && _frame->IsWindow())
		_frame->DestroyWindow();
	//delete _frame;
}

bool Factory::activate(Model::Channel *channel)
{
	std::map<Model::Channel *, ChannelFrame *>::iterator it = _mapChannels.find(channel);
	if (it != _mapChannels.end())
	{
		ChannelFrame *frame = it->second;
		frame->ShowWindow(SW_SHOW);
		frame->SetActiveWindow();
	}
	else
	{
		RECT rect = {200, 200, 840, 680};
		ChannelFrame *frame = new ChannelFrame(*this, channel);
		if ( frame->Create(rect) )
			_mapChannels[channel] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::deactivate(Model::Channel *channel)
{
	std::map<Model::Channel *, ChannelFrame *>::iterator it = _mapChannels.find(channel);
	if (it != _mapChannels.end())
	{
		_mapChannels.erase(it);
		return true;
	}
	return false;
}

bool Factory::activate(Model::User *user)
{
	std::map<Model::User *, UserFrame *>::iterator it = _mapUsers.find(user);
	if (it != _mapUsers.end())
	{
		UserFrame *frame = it->second;
		frame->ShowWindow(SW_SHOW);
		frame->SetActiveWindow();
	}
	else
	{
		RECT rect = {200, 200, 840, 680};
		UserFrame *frame = new UserFrame(*this, user);
		if ( frame->Create(rect) )
			_mapUsers[user] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::deactivate(Model::User *user)
{
	std::map<Model::User *, UserFrame *>::iterator it = _mapUsers.find(user);
	if (it != _mapUsers.end())
	{
		_mapUsers.erase(it);
		return true;
	}
	return false;
}

bool Factory::activate(Model::Group *group)
{
	std::map<Model::Group *, GroupFrame *>::iterator it = _mapGroups.find(group);
	if (it != _mapGroups.end())
	{
		GroupFrame *frame = it->second;
		frame->ShowWindow(SW_SHOW);
		frame->SetActiveWindow();
	}
	else
	{
		RECT rect = {200, 200, 840, 680};
		GroupFrame *frame = new GroupFrame(*this, group);
		if ( frame->Create(rect) )
			_mapGroups[group] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::deactivate(Model::Group *group)
{
	std::map<Model::Group *, GroupFrame *>::iterator it = _mapGroups.find(group);
	if (it != _mapGroups.end())
	{
		_mapGroups.erase(it);
		return true;
	}
	return false;
}

bool Factory::activate(Model::Site *site)
{
	std::map<Model::Site *, SiteFrame *>::iterator it = _mapSites.find(site);
	if (it != _mapSites.end())
	{
		SiteFrame *frame = it->second;
		frame->ShowWindow(SW_SHOW);
		frame->SetActiveWindow();
	}
	else
	{
		RECT rect = {200, 200, 840, 680};
		SiteFrame *frame = new SiteFrame(*this, site);
		if ( frame->Create(rect) )
			_mapSites[site] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::deactivate(Model::Site *site)
{
	std::map<Model::Site *, SiteFrame *>::iterator it = _mapSites.find(site);
	if (it != _mapSites.end())
	{
		_mapSites.erase(it);
		return true;
	}
	return false;
}

bool Factory::activate(Model::System *system)
{
	std::map<Model::System *, SystemFrame *>::iterator it = _mapSystems.find(system);
	if (it != _mapSystems.end())
	{
		SystemFrame *frame = it->second;
		frame->ShowWindow(SW_SHOW);
		frame->SetActiveWindow();
	}
	else
	{
		RECT rect = {200, 200, 840, 680};
		SystemFrame *frame = new SystemFrame(*this, system);
		if ( frame->Create(rect) )
			_mapSystems[system] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::deactivate(Model::System *system)
{
	std::map<Model::System *, SystemFrame *>::iterator it = _mapSystems.find(system);
	if (it != _mapSystems.end())
	{
		_mapSystems.erase(it);
		return true;
	}
	return false;
}

bool Factory::activate(Receiver *receiver)
{
	std::map<Receiver *, ReceiverFrame *>::iterator it = _mapReceivers.find(receiver);
	if (it != _mapReceivers.end())
	{
		ReceiverFrame *frame = it->second;
		frame->ShowWindow(SW_SHOW);
		frame->SetActiveWindow();
	}
	else
	{
		RECT rect = {200, 200, 840, 680};
		ReceiverFrame *frame = new ReceiverFrame(*this, receiver);
		if ( frame->Create(rect) )
			_mapReceivers[receiver] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::deactivate(Receiver *receiver)
{
	std::map<Receiver *, ReceiverFrame *>::iterator it = _mapReceivers.find(receiver);
	if (it != _mapReceivers.end())
	{
		_mapReceivers.erase(it);
		return true;
	}
	return false;
}

bool Factory::activate(int nCmdShow)
{
	if (_frame == NULL)
	{
		RECT rect = {0, 0, 480, 400};
		_frame = new FolderFrame(*this);
		bool bOK = _frame->Create(rect);
		if (bOK)
		{
			_frame->CenterWindow();
			_frame->ShowWindow(nCmdShow);
			return true;
		}
		else
		{
			// something bad happened.
			TCHAR log[64] = {0};
			DWORD last = GetLastError();
			wsprintf(log, _T("Main dialog creation failed, error %d!\n"), last);
			ATLTRACE(log);
			return false;
		}
	}
	_frame->ShowWindow(SW_SHOW);
	_frame->SetActiveWindow();
	return true;
}
