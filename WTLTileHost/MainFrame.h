#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Button.h"
#include "../Tile/Gauge.h"
#include "../WTL/Frame.h"
#include "../Model/Person.h"

struct MainFrame : public Frame
{
	MainFrame(Theme &theme);
	void clickTheme(Button *, bool bDown);
	void clickAbout(Button *, bool bDown);
	void clickOK(Button *, bool);
	void clickSnapshot(Button *, bool);
	// user clicked check
	void clickTick(Button *, bool bDown);
private:
	Window m_editor;
	Window m_about;
	ThemeSet m_set;
	Person m_person;
	PersonSet m_adapter;
	Gauge *m_gauge;
};

