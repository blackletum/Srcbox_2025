//The following include files are necessary to allow your MyPanel.cpp to compile.
#include "cbase.h"
#include "vgui_savenquit.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/ListPanel.h>
#include <filesystem.h>
#include <KeyValues.h>
#include "ienginevgui.h"
#include <vgui_controls/PanelListPanel.h>
#include <vgui_controls/ImagePanel.h>
#include "vgui_controls/Menu.h"
#include <vgui_controls/TextEntry.h>

#include <stdlib.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Used by the autocompletion system
//-----------------------------------------------------------------------------
class CNonFocusableMenu : public Menu
{
	DECLARE_CLASS_SIMPLE(CNonFocusableMenu, Menu);

public:
	CNonFocusableMenu(Panel *parent, const char *panelName)
		: BaseClass(parent, panelName),
		m_pFocus(0)
	{
	}

	void SetFocusPanel(Panel *panel)
	{
		m_pFocus = panel;
	}

	VPANEL GetCurrentKeyFocus()
	{
		if (!m_pFocus)
			return GetVPanel();

		return m_pFocus->GetVPanel();
	}

private:
	Panel		*m_pFocus;
};

class CSaveBeforeQuitQueryDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CSaveBeforeQuitQueryDialog, vgui::Frame);

public:
	CSaveBeforeQuitQueryDialog(vgui::VPANEL parent);
	~CSaveBeforeQuitQueryDialog() {}

protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand) override;

private:
	Button* Next;
	Button* Prev;
	Button* Play;
	Button* Close;
	Button* Cancel;
};

ConVar cancel("", "0", FCVAR_CLIENTDLL, "");

CSaveBeforeQuitQueryDialog::CSaveBeforeQuitQueryDialog(vgui::VPANEL parent)
	: BaseClass(nullptr, "SaveBeforeQuitQueryDialog")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(true);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetSizeable(false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(true);
	SetMoveable(true);
	SetTitle("#GameUI_Quit", false);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/savebeforequitdialog.res");

	SetVisible(false);
}

// Class for managing panel instance
class CSaveBeforeQuitQueryDialogInterface : public ISaveBeforeQuitDialog
{
private:
	CSaveBeforeQuitQueryDialog* m_pPanel;

public:
	CSaveBeforeQuitQueryDialogInterface()
		: m_pPanel(nullptr) {}

	void Create(vgui::VPANEL parent) override
	{
		if (!m_pPanel)
		{
			m_pPanel = new CSaveBeforeQuitQueryDialog(parent);
		}
	}

	void Destroy() override
	{
		if (m_pPanel)
		{
			m_pPanel->SetParent(nullptr);
			delete m_pPanel;
			m_pPanel = nullptr;
		}
	}

	void Activate() override
	{
		if (m_pPanel)
		{
			m_pPanel->Activate();
		}
	}
};

static CSaveBeforeQuitQueryDialogInterface g_SavebeforeQuit;
ISaveBeforeQuitDialog* savebeforequitdialog = (ISaveBeforeQuitDialog*)&g_SavebeforeQuit;

class CQuitQueryBoxDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CQuitQueryBoxDialog, vgui::Frame);

public:
	CQuitQueryBoxDialog(vgui::VPANEL parent);
	~CQuitQueryBoxDialog() {}

protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand) override;

private:
	void ImagePanel();
	vgui::Frame* m_pQuitPanel;
	vgui::Label* m_pQuitConfirmText;
	Button* m_pCancelButton;
	Button* m_pQuitButton;
};

CQuitQueryBoxDialog::CQuitQueryBoxDialog(vgui::VPANEL parent)
	: BaseClass(nullptr, "QuitQueryDialog")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetSizeable(false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetMoveable(true);
	SetTabPosition(2);
	SetTitle("#GameUI_QuitConfirmationTitle", false);

	m_pQuitPanel = new vgui::Frame(this, "QuitQueryDialog");
	SetBounds(655, 392, 290, 116);
	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	m_pQuitConfirmText = new vgui::Label(this, "", "#GameUI_QuitConfirmationText");
	m_pQuitConfirmText->SetBounds(54, 31, 190, 16);

	m_pQuitButton = new Button(this, "", "#GameUI_Quit");
	m_pQuitButton->SetCommand("OK");
	m_pQuitButton->SetTabPosition(1);
	m_pQuitButton->SetBounds(73, 69, 75, 26);
	m_pQuitButton->SetVisible(true);

	m_pCancelButton = new Button(this, "", "#QueryBox_Cancel");
	m_pCancelButton->SetCommand("OnCancel");
	//m_pCancelButton->SetTabPosition(2);
	m_pCancelButton->SetBounds(165, 69, 64, 26); // Adjust position and size
	m_pCancelButton->SetVisible(true);

	SetVisible(false);
}

// Class for managing panel instance
class CQuitQueryBoxDialogInterface : public IQuitQueryBoxDialog
{
private:
	CQuitQueryBoxDialog* m_pPanel;

public:
	CQuitQueryBoxDialogInterface()
		: m_pPanel(nullptr) {
	}

	void Create(vgui::VPANEL parent) override
	{
		if (!m_pPanel)
		{
			m_pPanel = new CQuitQueryBoxDialog(parent);
		}
	}

	void Destroy() override
	{
		if (m_pPanel)
		{
			m_pPanel->SetParent(nullptr);
			delete m_pPanel;
			m_pPanel = nullptr;
		}
	}

	void Activate() override
	{
		if (m_pPanel)
		{
			m_pPanel->Activate();
		}
	}
};

static CQuitQueryBoxDialogInterface g_QuitQueryBoxDialog;
IQuitQueryBoxDialog* quitquerybox = (IQuitQueryBoxDialog*)&g_QuitQueryBoxDialog;

void CSaveBeforeQuitQueryDialog::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cancel.GetBool());
}


CON_COMMAND(OpenSaveBeforeQuitDialog, "")
{
	savebeforequitdialog->Activate();
};

CON_COMMAND(OpenQuitDialog, "")
{
	quitquerybox->Activate();
};

void CSaveBeforeQuitQueryDialog::OnCommand(const char* pcCommand)
{
	if (FStrEq(pcCommand, "Cancel"))
	{
		SetVisible(false);
	}

	if (FStrEq(pcCommand, "OK"))
	{
		engine->ClientCmd_Unrestricted("quit");
	}
}

void CQuitQueryBoxDialog::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cancel.GetBool());
}

void CQuitQueryBoxDialog::OnCommand(const char* pcCommand)
{
	if (FStrEq(pcCommand, "OnCancel"))
	{
		engine->ClientCmd_Unrestricted("quit");
		Close();
	}

	if (FStrEq(pcCommand, "OK"))
	{
		SetVisible(false);
		engine->ClientCmd_Unrestricted("quit");
	}
}

































