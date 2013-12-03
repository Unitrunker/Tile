#include "stdafx.h"
#include "Factory.h"
#include "Channel.h"
#include "Site.h"
#include "Group.h"
#include "User.h"
#include "Folder.h"
#include "Receiver.h"

static void Center(RECT &rect)
{
	RECT desktop = {0};
	HWND hDT = GetDesktopWindow();
	GetWindowRect(hDT, &desktop);
	rect.bottom -= rect.top;
	rect.right -= rect.left;
	rect.left = 0;
	rect.top = 0;
	rect.top = (desktop.bottom - rect.bottom) / 2;
	rect.left = (desktop.right - rect.right) / 2;
	rect.bottom += rect.top;
	rect.right += rect.left;
}

Factory::Factory() : _frame(NULL), _last(0)
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
		Center(rect);
		ChannelFrame *frame = new ChannelFrame(*this, channel);
		if ( frame->Create(rect) )
			_mapChannels[channel] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::activate(std::vector<Model::Channel *> &list)
{
	if (list.size() == 0)
		return false;

	if (list.size() == 1)
		return activate(list[0]);

	RECT rect = {200, 200, 840, 680};
	Center(rect);
	ChannelFrame *frame = new ChannelFrame(*this, list);
	bool bOK = frame->Create(rect);
	if (!bOK)
		delete frame;
	return bOK;
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
		Center(rect);
		UserFrame *frame = new UserFrame(*this, user);
		if ( frame->Create(rect) )
			_mapUsers[user] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::activate(std::vector<Model::User *> &list)
{
	if (list.size() == 0)
		return false;

	if (list.size() == 1)
		return activate(list[0]);

	RECT rect = {200, 200, 840, 680};
	Center(rect);
	UserFrame *frame = new UserFrame(*this, list);
	bool bOK = frame->Create(rect);
	if (!bOK)
		delete frame;
	return bOK;
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
		Center(rect);
		GroupFrame *frame = new GroupFrame(*this, group);
		if ( frame->Create(rect) )
			_mapGroups[group] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::activate(std::vector<Model::Group *> &list)
{
	if (list.size() == 0)
		return false;

	if (list.size() == 1)
		return activate(list[0]);

	RECT rect = {200, 200, 840, 680};
	Center(rect);
	GroupFrame *frame = new GroupFrame(*this, list);
	bool bOK = frame->Create(rect);
	if (!bOK)
		delete frame;
	return bOK;
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
		Center(rect);
		SiteFrame *frame = new SiteFrame(*this, site);
		if ( frame->Create(rect) )
			_mapSites[site] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::activate(std::vector<Model::Site *> &list)
{
	if (list.size() == 0)
		return false;

	if (list.size() == 1)
		return activate(list[0]);

	RECT rect = {200, 200, 840, 680};
	Center(rect);
	SiteFrame *frame = new SiteFrame(*this, list);
	bool bOK = frame->Create(rect);
	if (!bOK)
		delete frame;
	return bOK;
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
		Center(rect);
		SystemFrame *frame = new SystemFrame(*this, system);
		if ( frame->Create(rect) )
			_mapSystems[system] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::activate(std::vector<Model::System *> &list)
{
	if (list.size() == 0)
		return false;

	if (list.size() == 1)
		return activate(list[0]);

	RECT rect = {200, 200, 840, 680};
	Center(rect);
	SystemFrame *frame = new SystemFrame(*this, list);
	bool bOK = frame->Create(rect);
	if (!bOK)
		delete frame;
	return bOK;
}

// Activate a new, blank group that has no known identity.
bool Factory::activateChannel(Model::Site *site)
{
	RECT rect = {200, 200, 840, 680};
	Center(rect);
	ChannelFrame *frame = new ChannelFrame(*this, site);
	if ( frame->Create(rect) )
	{
		Model::Channel *channel = frame->_set.at(0);
		_mapChannels[channel] = frame;
		return true;
	}
	delete frame;
	return false;
}

// Activate a new, blank group that has no known identity.
bool Factory::activateGroup(Model::System *system)
{
	RECT rect = {200, 200, 840, 680};
	Center(rect);
	GroupFrame *frame = new GroupFrame(*this, system);
	if ( frame->Create(rect) )
	{
		Model::Group *group = frame->_set.at(0);
		_mapGroups[group] = frame;
		return true;
	}
	delete frame;
	return false;
}

// Activate a new, blank user that has no known identity.
bool Factory::activateUser(Model::System *system)
{
	RECT rect = {200, 200, 840, 680};
	Center(rect);
	UserFrame *frame = new UserFrame(*this, system);
	if ( frame->Create(rect) )
	{
		Model::User *user = frame->_set.at(0);
		_mapUsers[user] = frame;
		return true;
	}
	delete frame;
	return false;
}

// Activate a new, blank site that has no known identity.
bool Factory::activateSite(Model::System *system)
{
	RECT rect = {200, 200, 840, 680};
	Center(rect);
	SiteFrame *frame = new SiteFrame(*this, system);
	if ( frame->Create(rect) )
	{
		Model::Site *site = frame->_set.at(0);
		_mapSites[site] = frame;
		return true;
	}
	delete frame;
	return false;
}

// Activate a new, blank system that has no known identity.
bool Factory::activateSystem()
{
	RECT rect = {200, 200, 840, 680};
	Center(rect);
	SystemFrame *frame = new SystemFrame(*this);
	if ( frame->Create(rect) )
	{
		Model::System *system = frame->_set.at(0);
		_mapSystems[system] = frame;
		return true;
	}
	delete frame;
	return false;
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
		RECT rect = {0, 0, 480, 320};
		Center(rect);
		ReceiverFrame *frame = new ReceiverFrame(*this, receiver);
		if ( frame->Create(rect) )
			_mapReceivers[receiver] = frame;
		else
			delete frame;
	}
	return true;
}

bool Factory::activate(std::vector<Receiver *> &list)
{
	if (list.size() == 0)
		return false;

	if (list.size() == 1)
		return activate(list[0]);

	RECT rect = {0, 0, 480, 320};
	Center(rect);
	ReceiverFrame *frame = new ReceiverFrame(*this, list);
	bool bOK = frame->Create(rect);
	if (!bOK)
		delete frame;
	return bOK;
}

// Activate a new, blank receiver.
bool Factory::activateReceiver()
{
	RECT rect = {200, 200, 840, 680};
	Center(rect);
	ReceiverFrame *frame = new ReceiverFrame(*this);
	if ( frame->Create(rect) )
	{
		Receiver *receiver = frame->_set.at(0);
		_mapReceivers[receiver] = frame;
		return true;
	}
	delete frame;
	return false;
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
		Center(rect);
		_frame = new FolderFrame(*this);
		bool bOK = _frame->Create(rect);
		if (!bOK)
		{
			// something bad happened.
			TCHAR log[64] = {0};
			DWORD last = GetLastError();
			wsprintf(log, _T("Main dialog creation failed, error %d!\n"), last);
			ATLTRACE(log);
			return false;
		}
	}
	_frame->ShowWindow(nCmdShow);
	_frame->SetActiveWindow();
	return true;
}

unsigned long Factory::ident()
{
	return ++_last;
}