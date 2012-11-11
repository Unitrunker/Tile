#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../WTL/Frame.h"
#include "Model.h"
#include "System.h"

/*
Copyright © 2012 Rick Parrish
*/

struct FolderFrame : public Frame
{
	FolderFrame(Theme &theme);
private:
	void clickety(Grid *, size_t row, size_t col);

	Pane *_top;
	Tab *_tools;
	Table<SystemSet, Model::System> _table;
	Model::Folder _folder;
};
