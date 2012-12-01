#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Button.h"
#include "../Tile/Gauge.h"
#include "../WTL/Frame.h"
#include "../Model/Person.h"

struct MainFrame : public Frame
{
	MainFrame(Theme &theme);
private:
	void clickTheme(Button *);
	void clickAbout(Button *);
	void clickOK(Button *);
	void clickSnapshot(Button *);
	// user clicked check
	void clickTick(Button *);

	Window m_editor;
	Window m_about;
	ThemeSet m_set;
	Person m_person;
	PersonSet m_adapter;
	Gauge *m_gauge;
};

