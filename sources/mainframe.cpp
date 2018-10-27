#include "mainframe.h"

#include "main.h"
#include "dlgabout.h"
#include "appversion.h"
#include "dlgoptions.h"
#include "dlgaddeditbill.h"
#include "settingsmanager.h"
#include "panelnbookpage_home.h"
#include "panelnbookpage_bills.h"
#include "panelnbookpage_clients.h"
#include "panelnbookpage_estimates.h"

#include <wx/display.h>
#include <wx/notebook.h>

#ifndef __WXMSW__
    #include "../graphx/microbill.xpm"
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
    // Page "Clients"
    m_nbPage[PNBP_TYPE_CLIENTS]=new PanelNBookPage_Clients(m_nBook);
    m_nBook->AddPage(m_nbPage[PNBP_TYPE_CLIENTS], _("Clients"));
    // Page "Estimates"
    m_nbPage[PNBP_TYPE_ESTIMATES]=new PanelNBookPage_Estimates(m_nBook);
    m_nBook->AddPage(m_nbPage[PNBP_TYPE_ESTIMATES], _("Estimates"));
    // Page "Bills"
    m_nbPage[PNBP_TYPE_BILLS]=new PanelNBookPage_Bills(m_nBook);
    m_nBook->AddPage(m_nbPage[PNBP_TYPE_BILLS], _("Bills"));

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
    // Misc events handlers
    Bind(wxEVT_CONVERT_ESTIM2BILL, &MainFrame::OnConvertEstim2Bill, this);
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
    // Save the Money sign and position settins
    int iSignPos = m_settings.GetMoneySignPos();
    wxString sSign = m_settings.GetMonetarySign();

    DlgOptions dlg(this);
    dlg.ShowModal();

    if (m_settings.GetProhibitI18N()!=bOldLng)
    {
        wxMessageBox(_("You changed the translation settings.\nYou must restart the application to see this in effect."), _("Restart needed"), wxICON_INFORMATION|wxCENTER|wxOK);
    }
    if ((m_settings.GetMoneySignPos() != iSignPos) ||
        (m_settings.GetMonetarySign() != sSign) )
    {
        m_nbPage[PNBP_TYPE_ESTIMATES]->RefreshDatasLists();
        m_nbPage[PNBP_TYPE_BILLS]->RefreshDatasLists();
    }
}

void MainFrame::OnExitClicked(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAboutClicked(wxCommandEvent& event)
{
    DlgAbout dlg(this);
    dlg.ShowModal();
}

void MainFrame::OnConvertEstim2Bill(wxCommandEvent& event)
{
    Estimate* eItem=(Estimate*)event.GetEventObject();
    if (eItem==NULL) return;

    DatasManager& datas=DatasManager::Get();

    Bill* bItem=datas.AddNewBill(eItem);
    DlgAddEditBill dlg(this, bItem);
    if (dlg.ShowModal()!=wxID_OK)
    {
        datas.RemoveBill(bItem);
        return;
    }
    PanelNBookPage_Bills* page=(PanelNBookPage_Bills*)m_nbPage[PNBP_TYPE_BILLS];
    page->AddBillItem(bItem, true);
    m_nBook->SetSelection(PNBP_TYPE_BILLS);
}
