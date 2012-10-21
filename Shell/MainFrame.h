#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"

struct MainFrame : public Frame
{
	MainFrame(Theme &theme);
	void clickTheme(Button *, bool bDown);
	void clickAbout(Button *, bool bDown);
	void clickOK(Button *, bool);
	void clickSnapshot(Button *, bool);
private:
	Window m_editor;
	Window m_about;
	ThemeSet m_set;
	Value<string_t> m_value;
};

