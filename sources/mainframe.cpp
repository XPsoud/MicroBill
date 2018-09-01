#include "mainframe.h"

#include "main.h"
#include "appversion.h"
#include "dlgoptions.h"
#include "settingsmanager.h"
#include "panelnbookpage_home.h"

#include <wx/display.h>
#include <wx/notebook.h>
#include <wx/aboutdlg.h>

#ifndef __WXMSW__
    #include "../graphx/wxwin.xpm"
#endif // __WXMSW__

MainFrame::MainFrame(const wxString& title) : wxFrame(NULL, -1, title),
    m_settings(SettingsManager::Get())
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"MainFrame\" object\n"));
#endif // __WXDEBUG__

    SetIcon(wxICON(appIcon));

    CreateControls();

    int iStartPos = m_settings.GetMainWndStartupPos();
    wxSize szStartSize = m_settings.GetMainWndStartupSize();
    wxPoint ptStartPos;
    m_settings.GetMainWndStartupPos(ptStartPos);
    if (iStartPos==wxALIGN_NOT)
    {
        if (szStartSize == wxDefaultSize)
        {
            if (ptStartPos == wxDefaultPosition)
            {
                Maximize(true);
            }
            else
            {
                SetSize(MAINFRAME_MIN_SIZE);
                CenterOnScreen();
            }
        }
        else
        {
            Move(ptStartPos);
            SetSize(szStartSize);
        }
    }
    else
    {

        wxDisplay d;
        wxRect rcD=d.GetClientArea();
        int iWScr=rcD.GetWidth();
        int iHScr=rcD.GetHeight();
        wxPoint pt=wxDefaultPosition;
        if (szStartSize == wxDefaultSize)
            szStartSize = MAINFRAME_MIN_SIZE;
        pt.x=(((iStartPos&wxLEFT)==wxLEFT)?0:((iStartPos&wxRIGHT)==wxRIGHT)?iWScr-szStartSize.GetWidth():(iWScr-szStartSize.GetWidth())/2);
        pt.y=(((iStartPos&wxTOP)==wxTOP)?0:((iStartPos&wxBOTTOM)==wxBOTTOM)?iHScr-szStartSize.GetHeight():(iHScr-szStartSize.GetHeight())/2);
        Move(pt);
        SetSize(szStartSize);
    }
    SetMinSize(MAINFRAME_MIN_SIZE);

    ConnectControls();
}


MainFrame::~MainFrame()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"MainFrame\" object\n"));
#endif // __WXDEBUG__
}

void MainFrame::CreateControls()
{
    CreateStatusBar(2);
    SetStatusText(wxGetApp().GetVersionString(true, true), 1);


    wxPanel *pnlMain=new wxPanel(this, -1);
    wxBoxSizer *szrMain=new wxBoxSizer(wxVERTICAL);
        m_nBook=new wxNotebook(pnlMain, -1);
        szrMain->Add(m_nBook, 1, wxALL|wxEXPAND, 0);
    pnlMain->SetSizer(szrMain);

    // Page "Home"
    m_nbPage[PNBP_TYPE_HOME]=new PanelNBookPage_Home(m_nBook);
    m_nBook->AddPage(m_nbPage[PNBP_TYPE_HOME], _("Home"));

    // Menu bar
    wxMenuBar* mbar = new wxMenuBar();
        wxMenu* fileMenu = new wxMenu(_T(""));
            fileMenu->Append(wxID_PREFERENCES);
            fileMenu->AppendSeparator();
            fileMenu->Append(wxID_EXIT);
        mbar->Append(fileMenu, wxGetStockLabel(wxID_FILE));

        wxMenu* helpMenu = new wxMenu(_T(""));
            helpMenu->Append(wxID_ABOUT);
        mbar->Append(helpMenu, wxGetStockLabel(wxID_HELP));
    SetMenuBar(mbar);
}

void MainFrame::ConnectControls()
{
    // General events handlers
    Bind(wxEVT_SIZE, &MainFrame::OnSize, this);
    Bind(wxEVT_MOVE, &MainFrame::OnMove, this);
    // Menus items
    Bind(wxEVT_MENU, &MainFrame::OnPrefsClicked, this, wxID_PREFERENCES);
    Bind(wxEVT_MENU, &MainFrame::OnExitClicked, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::OnAboutClicked, this, wxID_ABOUT);
}

void MainFrame::OnSize(wxSizeEvent& event)
{
    if (!IsShown()) return;
    if (IsMaximized())
    {
        m_settings.SetLastWindowRect(wxDefaultPosition, wxDefaultSize);
    }
    else
    {
        m_settings.SetLastWindowRect(GetPosition(), GetSize());
    }
    event.Skip();
}

void MainFrame::OnMove(wxMoveEvent& event)
{
    if (!IsShown()) return;
    m_settings.SetLastWindowRect(GetPosition(), GetSize());
    event.Skip();
}

void MainFrame::OnPrefsClicked(wxCommandEvent &event)
{
    // Save the "KeepLang" state
    bool bOldLng=m_settings.GetProhibitI18N();

    DlgOptions dlg(this);
    dlg.ShowModal();

    if (m_settings.GetProhibitI18N()!=bOldLng)
    {
        wxMessageBox(_("You changed the translation settings.\nYou must restart the application to see this in effect."), _("Restart needed"), wxICON_INFORMATION|wxCENTER|wxOK);
    }
}

void MainFrame::OnExitClicked(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAboutClicked(wxCommandEvent& event)
{
    wxAboutDialogInfo info;

    info.SetName(_T(PRODUCTNAME));
    info.SetVersion(wxGetApp().GetVersionNumString(true));
    info.SetDescription(wDESCRIPTION);
    info.SetCopyright(COPYRIGHT);
    info.SetIcon(wxICON(appIcon));

    wxAboutBox(info);
}
