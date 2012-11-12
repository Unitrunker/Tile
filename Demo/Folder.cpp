#include "stdafx.h"
#include "Folder.h"
#include "../Tile/Grid.h"
#include "../Tile/Fill.h"
#include "../Tile/Text.h"
#include "../Tile/Edit.h"
#include "../WTL/Snapshot.h"
#include <stdlib.h>

FolderFrame::FolderFrame(Theme &theme) : 
	Frame(theme), _table(theme)
{
	theme.setHeight(14);

	size_t columns = _table.getHeader()->Columns.size();

	_table.setReadOnly(true);

	for (size_t i = 0; i < columns; i++)
	{
		_table.setContent(i, &_folder.Systems);
	}

	time_t now = 0;
	time(&now);

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

	sophia::delegate2<void, Button*, bool> click;

	_top = new Pane(0, theme, eDown);
	_tools = new Tab(0, theme);

	Font webdings(_T("Webdings"), 24, 1);
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font = { Theme::eDefault, segoe };

	_tools->Add(L"\x231A", font, click); // watch
	_tools->Add(L"\x231B", font, click); // hourglass
	_tools->Add(L"\x2328", font, click); // keyboard
	_tools->Add(L"\x2388", font, click); // Helm
	_tools->Add(L"\x2622", font, click); // radioactive
	_tools->Add(L"\x2623", font, click); // biohazard

	font.font = webdings;
	_tools->Add(L"\x22", font, click); // web
	_tools->Add(L"\x40", font, click); // tools
	_tools->Add(L"\x71", font, click); // refresh
	_tools->Add(L"\x72", font, click); // X
	_tools->Add(L"\x73", font, click); // ?

	Grid *pGrid = new Grid(0, theme);
	pGrid->setTable(&_table);

	pGrid->DoubleClick.bind(this, &FolderFrame::clickety);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(pGrid, 0, 4096, 1, false);
	// connect the form to the frame window.
	setPane(_top);
}

void FolderFrame::clickety(Grid *, size_t row, size_t)
{
	Model::System *system = _folder.Systems[row];
	Theme &theme = getTheme();
	RECT rect = {200, 200, 840, 680};
	SystemFrame *frame = new SystemFrame(theme, system);
	frame->Create(m_hWnd, rect, _T("System X"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW);
}
