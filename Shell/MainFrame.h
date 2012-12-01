#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"

struct MainFrame : public Frame
{
	MainFrame(Theme &theme);
	void clickTheme(Button *);
	void clickAbout(Button *);
	void clickOK(Button *);
	void clickSnapshot(Button *);
private:
	Window m_editor;
	Window m_about;
	ThemeSet m_set;
	Value<string_t> m_value;
};

