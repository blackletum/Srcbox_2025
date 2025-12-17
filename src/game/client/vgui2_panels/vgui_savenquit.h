// IMyPanel.h
class ISaveBeforeQuitDialog
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
};

extern ISaveBeforeQuitDialog* savebeforequitdialog;

class IQuitQueryBoxDialog
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
};

extern IQuitQueryBoxDialog* quitquerybox;