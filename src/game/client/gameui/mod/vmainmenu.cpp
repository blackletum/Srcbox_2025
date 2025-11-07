//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "VMainMenu.h"
#include "EngineInterface.h"
#include "VFooterPanel.h"
#include "VHybridButton.h"
#include "VFlyoutMenu.h"
#include "vGenericConfirmation.h"
#include "basemodpanel.h"
#include "VGuiSystemModuleLoader.h"

#include "vgui/ILocalize.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/Tooltip.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Image.h"

#include "steam/steam_api.h"
#include "materialsystem/materialsystem_config.h"

#include "ienginevgui.h"
#include "basepanel.h"
#include "vgui/ISurface.h"
#include "tier0/icommandline.h"
#include "fmtstr.h"
#include "filesystem.h"

#include "vgui/IVGui.h"

#include "KeyValues.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace BaseModUI;

//=============================================================================
// Enabled!!!! until we fix swarm ui, DO NOT DISABLE
static ConVar ui_old_options_menu( "ui_old_options_menu", "0", FCVAR_HIDDEN, "Brings up the old tabbed options dialog from Keyboard/Mouse when set to 1." );

void OpenGammaDialog( VPANEL parent );

//=============================================================================
MainMenu::MainMenu(Panel* parent, const char* panelName) :
	BaseClass(parent, panelName, true, false, false, false)
{
	SetProportional( true );
	SetTitle( "", false );
	SetMoveable( false );
	SetSizeable( false );

	SetLowerGarnishEnabled( false );

	//AddFrameListener( this );

	m_iQuickJoinHelpText = MMQJHT_NONE;

	SetDeleteSelfOnClose( true );

	//int x, y, w, h;
	//parent->GetBounds(x, y, w, h);

	vgui::ImagePanel* pGameBackground = new vgui::ImagePanel(this, "Back");
#ifdef HL2MP
	pGameBackground->SetImage("../console/background01_swarmmenu");
#else
	pGameBackground->SetImage("../vgui/appchooser/background_tf_widescreen");
#endif
	//pGameLogo2->SetImage("../console/background_menu_TEST");
	//pGameBackground->SetPos(0, 0);
	//pGameBackground->SetSize(1600, 900);
	//pGameBackground->SetShouldScaleImage(1);
	//pGameBackground->SetScaleAmount(1);

	//int screenWide, screenTall;
	//static int s_nBGTextureID = -1;
	//bool				m_bFullscreenPoster;

	/*int x, y, wide, tall;
	pGameBackground->GetBounds(x, y, wide, tall);
	surface()->DrawSetColor(Color(255, 255, 255, 255));
	s_nBGTextureID = vgui::surface()->CreateNewTextureID(true); // procedural
	vgui::surface()->DrawSetTextureFile(s_nBGTextureID, "vgui/appchooser/background_tf_widescreen", true, 0);
	surface()->DrawTexturedRect(x, y, x + wide, y + tall);*/

	const char* pszBackgroundImage;
	int screenWide, screenTall;
	surface()->GetScreenSize(screenWide, screenTall);
	float aspectRatio = (float)screenWide / (float)screenTall;
	bool bIsWidescreen = aspectRatio >= 1.5999f;

	pszBackgroundImage = (m_bFullscreenPoster && bIsWidescreen) ? "appchooser/background_tf_widescreen" : "appchooser/background_tf_widescreen";
}

//=============================================================================
void MainMenu::MainMenuLogo(vgui::EditablePanel* parent)
{
	/*Msg("[SWARMUI] Space HEY!\n");
	KeyValues* pModData = new KeyValues("ModData");
	if (pModData)
	{
		if (pModData->LoadFromFile(g_pFullFileSystem, "gameinfo.txt"))
		{
			const char* title = pModData->GetString("gamelogo", "0");
			if (Q_stricmp(pModData->GetString("gamelogo", "0"), "0") == 1);
			{
				const char* pSettings = "Resource/GameLogo.res";
				Msg("[SWARMUI] Space HEY!\n");
			}
		}
	}*/
}

//=============================================================================
MainMenu::~MainMenu()
{
	RemoveFrameListener( this );

}

//=============================================================================
void MainMenu::OnCommand( const char *command )
{
	bool bOpeningFlyout = false;
	if ( !Q_strcmp( command, "DeveloperCommentary" ) )
	{
		// Explain the rules of commentary
		GenericConfirmation* confirmation = 
			static_cast< GenericConfirmation* >( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, this, false ) );

		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#GAMEUI_CommentaryDialogTitle";
		data.pMessageText = "#L4D360UI_Commentary_Explanation";

		data.bOkButtonEnabled = true;
		data.pfnOkCallback = &AcceptCommentaryRulesCallback;
		data.bCancelButtonEnabled = true;

		confirmation->SetUsageData(data);
		NavigateFrom();
	}
	else if ( !Q_strcmp( command, "FlmExtrasFlyoutCheck" ) )
	{
		OnCommand( "FlmExtrasFlyout_Simple" );
		return;
	}
	else if (!Q_strcmp(command, "AudioVideo"))
	{
		if ( m_ActiveControl )
		{
			m_ActiveControl->NavigateFrom( );
		}
		CBaseModPanel::GetSingleton().OpenWindow(WT_AUDIOVIDEO, this, true );
	}
	else if (!Q_strcmp(command, "Srcbox_Singleplayer_Menu"))
	{
		engine->ClientCmd("srcbox_singleplayer");
	}
	else if (!Q_strcmp(command, "NewGameDialog"))
	{
		engine->ClientCmd("OpenNewGameDialog");
	}
	else if (!Q_strcmp(command, "OpenFriendsDialog"))
	{
		engine->ClientCmd("options_legacy");
	}
	else if (!Q_strcmp(command, "OpenLegacyOptions"))
	{
		CBaseModPanel::GetSingleton().OpenOptionsDialog(this);
	}
	else if (!Q_strcmp(command, "QuitGame"))
	{
		GenericConfirmation* confirmation = 
			static_cast< GenericConfirmation* >( CBaseModPanel::GetSingleton().OpenWindow( WT_GENERICCONFIRMATION, this, false ) );

		GenericConfirmation::Data_t data;

		data.pWindowTitle = "#L4D360UI_MainMenu_Quit_Confirm";
		data.pMessageText = "#L4D360UI_MainMenu_Quit_ConfirmMsg";

		data.bOkButtonEnabled = true;
		data.pfnOkCallback = &AcceptQuitGameCallback;
		data.bCancelButtonEnabled = true;

		confirmation->SetUsageData(data);

		NavigateFrom();
	}
	else if ( !Q_stricmp( command, "QuitGame_NoConfirm" ) )
	{
		engine->ClientCmd( "quit" );
	}
	else if (!Q_strcmp(command, "Audio"))
	{
		if ( ui_old_options_menu.GetBool() )
		{
			CBaseModPanel::GetSingleton().OpenOptionsDialog( this );
		}
		else
		{
			// audio options dialog, PC only
			if ( m_ActiveControl )
			{
				m_ActiveControl->NavigateFrom( );
			}
			CBaseModPanel::GetSingleton().OpenWindow(WT_AUDIO, this, true );
		}
	}
	else if (!Q_strcmp(command, "Video"))
	{
		if ( ui_old_options_menu.GetBool() )
		{
			CBaseModPanel::GetSingleton().OpenOptionsDialog( this );
			//engine->ClientCmd("Options");
		}
		else
		{
			// video options dialog, PC only
			if ( m_ActiveControl )
			{
				m_ActiveControl->NavigateFrom( );
			}
			CBaseModPanel::GetSingleton().OpenWindow(WT_VIDEO, this, true );
		}
	}
	else if (!Q_strcmp(command, "Brightness"))
	{
		if ( ui_old_options_menu.GetBool() )
		{
			CBaseModPanel::GetSingleton().OpenOptionsDialog( this );
		}
		else
		{
			// brightness options dialog, PC only
			OpenGammaDialog( GetVParent() );
		}
	}
	else if (!Q_strcmp(command, "KeyboardMouse"))
	{
		if ( ui_old_options_menu.GetBool() )
		{
			CBaseModPanel::GetSingleton().OpenOptionsDialog( this );
		}
		else
		{
			// standalone keyboard/mouse dialog, PC only
			if ( m_ActiveControl )
			{
				m_ActiveControl->NavigateFrom( );
			}
			CBaseModPanel::GetSingleton().OpenWindow(WT_KEYBOARDMOUSE, this, true );
		}
	}
	else if( Q_stricmp( "#L4D360UI_Controller_Edit_Keys_Buttons", command ) == 0 )
	{
		FlyoutMenu::CloseActiveMenu();
		CBaseModPanel::GetSingleton().OpenKeyBindingsDialog( this );
	}
	else if (!Q_strcmp(command, "MultiplayerSettings"))
	{
		if ( ui_old_options_menu.GetBool() )
		{
			CBaseModPanel::GetSingleton().OpenOptionsDialog( this );
		}
		else
		{
			// standalone multiplayer settings dialog, PC only
			if ( m_ActiveControl )
			{
				m_ActiveControl->NavigateFrom( );
			}
			CBaseModPanel::GetSingleton().OpenWindow(WT_MULTIPLAYER, this, true );
		}
	}
	else if ( !Q_strcmp( command, "OpenServerBrowser" ) )
	{
		if ( CheckAndDisplayErrorIfNotLoggedIn() )
			return;

		// on PC, bring up the server browser and switch it to the LAN tab (tab #5)
		engine->ClientCmd( "openserverbrowser" );
	}
	else if ( !Q_strcmp( command, "OpenCreateMultiplayerGameDialog" ) )
	{
		CBaseModPanel::GetSingleton().OpenCreateMultiplayerGameDialog( this );
	}
	else if (command && command[0] == '#')
	{
		// Pass it straight to the engine as a command
		engine->ClientCmd( command+1 );
	}
	else
	{
		const char *pchCommand = command;
		if ( !Q_strcmp(command, "FlmOptionsFlyout") )
		{
		}
		else if ( !Q_strcmp(command, "FlmVersusFlyout") )
		{
			command = "VersusSoftLock";
		}
		else if ( !Q_strcmp( command, "FlmSurvivalFlyout" ) )
		{
			command = "SurvivalCheck";
		}
		else if ( !Q_strcmp( command, "FlmScavengeFlyout" ) )
		{
			command = "ScavengeCheck";
		}
		else if ( StringHasPrefix( command, "FlmExtrasFlyout_" ) )
		{
			command = "FlmExtrasFlyoutCheck";
		}

		// does this command match a flyout menu?
		BaseModUI::FlyoutMenu *flyout = dynamic_cast< FlyoutMenu* >( FindChildByName( pchCommand ) );
		if ( flyout )
		{
			bOpeningFlyout = true;

			// If so, enumerate the buttons on the menu and find the button that issues this command.
			// (No other way to determine which button got pressed; no notion of "current" button on PC.)
			for ( int iChild = 0; iChild < GetChildCount(); iChild++ )
			{
				bool bFound = false;
				/*
				GameModes *pGameModes = dynamic_cast< GameModes *>( GetChild( iChild ) );
				if ( pGameModes )
				{
					for ( int iGameMode = 0; iGameMode < pGameModes->GetNumGameInfos(); iGameMode++ )
					{
						BaseModHybridButton *pHybrid = pGameModes->GetHybridButton( iGameMode );
						if ( pHybrid && pHybrid->GetCommand() && !Q_strcmp( pHybrid->GetCommand()->GetString( "command"), command ) )
						{
							pHybrid->NavigateFrom();
							// open the menu next to the button that got clicked
							flyout->OpenMenu( pHybrid );
							flyout->SetListener( this );
							bFound = true;
							break;
						}
					}
				}*/

				if ( !bFound )
				{
					BaseModHybridButton *hybrid = dynamic_cast<BaseModHybridButton *>( GetChild( iChild ) );
					if ( hybrid && hybrid->GetCommand() && !Q_strcmp( hybrid->GetCommand()->GetString( "command"), command ) )
					{
						hybrid->NavigateFrom();
						// open the menu next to the button that got clicked
						flyout->OpenMenu( hybrid );
						flyout->SetListener( this );
						break;
					}
				}
			}
		}
		else
		{
			BaseClass::OnCommand( command );
		}
	}

	if( !bOpeningFlyout )
	{
		FlyoutMenu::CloseActiveMenu(); //due to unpredictability of mouse navigation over keyboard, we should just close any flyouts that may still be open anywhere.
	}
}

//=============================================================================
void MainMenu::OpenMainMenuJoinFailed( const char *msg )
{
}

//=============================================================================
void MainMenu::OnNotifyChildFocus( vgui::Panel* child )
{
}

void MainMenu::OnFlyoutMenuClose( vgui::Panel* flyTo )
{
	SetFooterState();
}

void MainMenu::OnFlyoutMenuCancelled()
{
}

//=============================================================================
void MainMenu::OnKeyCodePressed( KeyCode code )
{
	switch( GetBaseButtonCode( code ) )
	{
	case KEY_XBUTTON_B:
		// Capture the B key so it doesn't play the cancel sound effect
		break;
	default:
		BaseClass::OnKeyCodePressed( code );
		break;
	}
}

//=============================================================================
void MainMenu::OnThink()
{

	FlyoutMenu *pFlyout = dynamic_cast< FlyoutMenu* >( FindChildByName( "FlmOptionsFlyout" ) );
	if ( pFlyout )
	{
		const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();
		pFlyout->SetControlEnabled( "BtnBrightness", !config.Windowed() );
	}

	BaseClass::OnThink();
}

//=============================================================================
void MainMenu::OnOpen()
{
	BaseClass::OnOpen();

	SetFooterState();

}

//=============================================================================
void MainMenu::RunFrame()
{
	BaseClass::RunFrame();
}

//=============================================================================
void MainMenu::PaintBackground() 
{
}

void MainMenu::SetFooterState()
{
	CBaseModFooterPanel *footer = BaseModUI::CBaseModPanel::GetSingleton().GetFooterPanel();
	if ( footer )
	{
		CBaseModFooterPanel::FooterButtons_t buttons = FB_ABUTTON;

		footer->SetButtons( buttons, FF_MAINMENU, false );
		footer->SetButtonText( FB_ABUTTON, "#L4D360UI_Select" );
		footer->SetButtonText( FB_XBUTTON, "#L4D360UI_MainMenu_SeeAll" );
	}
}

//=============================================================================
void MainMenu::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	const char* pSettings = "Resource/UI/BaseModUI/MainMenu.res";

	if (CommandLine()->FindParm("-gamepadui")) {
		return;
	}
	else {
		LoadControlSettings(pSettings);
	}

	int nParentW, nParentH;
	GetParent()->GetSize(nParentW, nParentH);
	SetBounds(0, 0, nParentW, nParentH);

	/*vgui::ImagePanel* pGameLogo = new vgui::ImagePanel(this, "gamelogo");
	const char* pImage = pScheme->GetResourceString("gamelogo");
	if (pImage && *pImage)
		pGameLogo->SetImage(pImage);
	m_hLogoFont = pScheme->GetFont("Logo.Font", true);*/

	vgui::ImagePanel* pGameLogo = new vgui::ImagePanel(this, "Logo");
#ifdef HL2MP
	pGameLogo->SetImage("../logo/srcbox_logo");
	pGameLogo->SetPos(170, 300);
	pGameLogo->SetSize(512, 128);
	pGameLogo->SetShouldScaleImage(1);
	pGameLogo->SetScaleAmount(0.85);
#else
	pGameLogo->SetImage("../vgui/menu_header");
	//pGameLogo->SetPos(0, 0);
	//pGameLogo->SetSize(1024, 128);
	//pGameLogo->SetShouldScaleImage(1);
	//pGameLogo->SetScaleAmount(0.85);

	int screenW, screenH;
	vgui::surface()->GetScreenSize(screenW, screenH);

	float scale = (float)screenW / 1920.0f;
	int logoW = (int)(2048 * scale);
	int logoH = (int)(256 * scale);

	pGameLogo->SetScaleAmount(1.5);
	pGameLogo->SetSize(logoW, logoH);
	pGameLogo->SetPos((screenW - logoW) / 5, 10);
	pGameLogo->SetShouldScaleImage(true);
#endif


	/*const char* pImage = pScheme->GetResourceString("Logo.Image");
	if (pImage && *pImage)
		m_LogoImage.SetImage(pImage);
	m_hLogoFont = pScheme->GetFont("Logo.Font", true);

	KeyValues* pModData = new KeyValues("ModData");
	if (pModData)
	{
		if (pModData->LoadFromFile(g_pFullFileSystem, "gameinfo.txt"))
		{
			m_LogoText[0] = pModData->GetString("title", pModData->GetString("title"));
			m_LogoText[1] = pModData->GetString("title2", pModData->GetString("title2"));
		}
		pModData->deleteThis();
	}

	vgui::surface()->DrawSetTextColor(255, 255, 255, 255);
	vgui::surface()->DrawSetTextFont(m_hLogoFont);

	int nMaxLogosW = 0, nTotalLogosH = 0;
	int nLogoW[2], nLogoH[2];
	for (int i = 0; i < 2; i++)
	{
		nLogoW[i] = 0;
		nLogoH[i] = 0;
		if (!m_LogoText[i].IsEmpty())
			vgui::surface()->GetTextSize(m_hLogoFont, m_LogoText[i].String(), nLogoW[i], nLogoH[i]);
		nMaxLogosW = Max(nLogoW[i], nMaxLogosW);
		nTotalLogosH += nLogoH[i];
	}

	int nLogoY = GetTall() - (50 + nTotalLogosH);

	if (m_LogoImage.IsValid())
	{
		int nY1 = 50;
		int nY2 = nY1 + nLogoH[0];
		int nX1 = 50;
		int nX2 = nX1 + (nLogoH[0] * 3);
		vgui::surface()->DrawSetColor(Color(255, 255, 255, 255));
		vgui::surface()->DrawSetTexture(m_LogoImage);
		vgui::surface()->DrawTexturedRect(nX1, nY1, nX2, nY2);
		vgui::surface()->DrawSetTexture(0);
	}
	else
	{
		for (int i = 1; i >= 0; i--)
		{
			vgui::surface()->DrawSetTextPos(50, 50);
			vgui::surface()->DrawPrintText(m_LogoText[i].String(), m_LogoText[i].Length());

			nLogoY -= nLogoH[i];
		}
	}*/

	BaseModHybridButton *button = dynamic_cast< BaseModHybridButton* >( FindChildByName( "BtnPlaySolo" ) );
	if (button)
	{
#ifdef _X360
		button->SetText( ( XBX_GetNumGameUsers() > 1 ) ? ( "#L4D360UI_MainMenu_PlaySplitscreen" ) : ( "#L4D360UI_MainMenu_PlaySolo" ) );
		button->SetHelpText((XBX_GetNumGameUsers() > 1) ? ("#L4D360UI_MainMenu_OfflineCoOp_Tip") : ("#L4D360UI_MainMenu_PlaySolo_Tip"));
#endif
}

	if (IsPC())
	{
		FlyoutMenu *pFlyout = dynamic_cast< FlyoutMenu* >(FindChildByName("FlmOptionsFlyout"));
		if (pFlyout)
		{
			bool bUsesCloud = false;

#ifdef IS_WINDOWS_PC
			//ISteamRemoteStorage *pRemoteStorage = SteamClient()?(ISteamRemoteStorage *)SteamClient()->GetISteamGenericInterface(
				//SteamAPI_GetHSteamUser(), SteamAPI_GetHSteamPipe(), STEAMREMOTESTORAGE_INTERFACE_VERSION ):NULL;
#else
			ISteamRemoteStorage *pRemoteStorage = NULL;
			AssertMsg(false, "This branch run on a PC build without IS_WINDOWS_PC defined.");
#endif

			//int32 availableBytes, totalBytes = 0;
			/*if (pRemoteStorage && pRemoteStorage->GetQuota(&totalBytes, &availableBytes))
			{
				if (totalBytes > 0)
				{
					bUsesCloud = true;
				}
			}*/

			pFlyout->SetControlEnabled("BtnCloud", bUsesCloud);
		}
	}

	SetFooterState();

}

void MainMenu::AcceptCommentaryRulesCallback() 
{
}

void MainMenu::AcceptQuitGameCallback()
{
	if ( MainMenu *pMainMenu = static_cast< MainMenu* >( CBaseModPanel::GetSingleton().GetWindow( WT_MAINMENU ) ) )
	{
		pMainMenu->OnCommand( "QuitGame_NoConfirm" );
	}
}

void MainMenu::AcceptVersusSoftLockCallback()
{
	if ( MainMenu *pMainMenu = static_cast< MainMenu* >( CBaseModPanel::GetSingleton().GetWindow( WT_MAINMENU ) ) )
	{
		pMainMenu->OnCommand( "FlmVersusFlyout" );
	}
}


CON_COMMAND_F( openserverbrowser, "Opens server browser", 0 )
{
	bool isSteam = steamapicontext->SteamFriends() && steamapicontext->SteamUtils();
	if ( isSteam )
	{
		// show the server browser
		g_VModuleLoader.ActivateModule("Servers");

		// if an argument was passed, that's the tab index to show, send a message to server browser to switch to that tab
		if ( args.ArgC() > 1 )
		{
			KeyValues *pKV = new KeyValues( "ShowServerBrowserPage" );
			pKV->SetInt( "page", atoi( args[1] ) );
			g_VModuleLoader.PostMessageToAllModules( pKV );
		}

#ifdef INFESTED_DLL
		KeyValues *pSchemeKV = new KeyValues( "SetCustomScheme" );
		pSchemeKV->SetString( "SchemeName", "SwarmServerBrowserScheme" );
		g_VModuleLoader.PostMessageToAllModules( pSchemeKV );
#else
		KeyValues *pSchemeKV = new KeyValues( "SetCustomScheme" );
		pSchemeKV->SetString( "SchemeName", "SourceScheme" );
		g_VModuleLoader.PostMessageToAllModules( pSchemeKV );
#endif
	}
}

