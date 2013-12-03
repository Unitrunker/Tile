#include "Model.h"
#include "../Tile/Theme.h"
#include <map>

#pragma once

struct SystemFrame;
struct SiteFrame;
struct GroupFrame;
struct UserFrame;
struct ChannelFrame;
struct FolderFrame;
struct ReceiverFrame;
struct Receiver;

// Various frame windows don't know about each other so they meet here.
struct Factory
{
	Factory();
	virtual ~Factory();

	// None of these are re-entrant. Call from UI thread.

	// Present main UI.
	bool activate(int nCmdShow);

	// Present UI for this channel. If one already 
	// exists, show it. Do not create another.
	bool activate(Model::Channel *);
	bool activate(std::vector<Model::Channel *> &);
	// Activate a new, blank channel that has no known identity.
	bool activateChannel(Model::Site *site);
	bool deactivate(Model::Channel *);

	// Present UI for this user. If one already 
	// exists, show it. Do not create another.
	bool activate(Model::User *);
	bool activate(std::vector<Model::User *> &);
	// Activate a new, blank user that has no known identity.
	bool activateUser(Model::System *system);
	bool deactivate(Model::User *);

	// Present UI for this group. If one already 
	// exists, show it. Do not create another.
	bool activate(Model::Group *);
	bool activate(std::vector<Model::Group *> &);
	// Activate a new, blank group that has no known identity.
	bool activateGroup(Model::System *system);
	bool deactivate(Model::Group *);

	// Present UI for this site. If one already 
	// exists, show it. Do not create another.
	bool activate(Model::Site *);
	bool activate(std::vector<Model::Site *> &);
	// Activate a new, blank site that has no known identity.
	bool activateSite(Model::System *system);
	bool deactivate(Model::Site *);

	// Present UI for this system. If one already 
	// exists, show it. Do not create another.
	bool activate(Model::System *);
	bool activate(std::vector<Model::System *> &);
	// Activate a new, blank system that has no known identity.
	bool activateSystem();
	bool deactivate(Model::System *);

	// Present UI for this receiver. If one already 
	// exists, show it. Do not create another.
	bool activate(Receiver *);
	bool activate(std::vector<Receiver *> &list);
	bool activateReceiver();
	bool deactivate(Receiver *);

	unsigned long ident();

	// Singleton for the main program window.
	FolderFrame* _frame;
	Model::Folder _folder;
	Tiles::Theme _theme;
private:
	unsigned long _last;
	// Collections of per-instance singleton windows.
	// Each item of data should have *at most* one frame window associated with it.
	std::map<Model::System *, SystemFrame *> _mapSystems;
	std::map<Model::Site *, SiteFrame *> _mapSites;
	std::map<Model::Group*, GroupFrame *> _mapGroups;
	std::map<Model::User *, UserFrame *> _mapUsers;
	std::map<Model::Channel *, ChannelFrame *> _mapChannels;
	std::map<Receiver *, ReceiverFrame *> _mapReceivers;
};

