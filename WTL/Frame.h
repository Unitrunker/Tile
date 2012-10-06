#include "Window.h"

class Frame : public Window
{
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
public:
	Frame(Theme &theme);
	~Frame();

	BEGIN_MSG_MAP(Frame)
		CHAIN_MSG_MAP(Window)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
};