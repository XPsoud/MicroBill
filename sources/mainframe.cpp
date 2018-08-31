#include "mainframe.h"

#include "main.h"

#ifndef __WXMSW__
    #include "../graphx/wxwin.xpm"
#endif // __WXMSW__

MainFrame::MainFrame(const wxString& title) : wxFrame(NULL, -1, title)
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"MainFrame\" object\n"));
#endif // __WXDEBUG__

    SetIcon(wxICON(appIcon));

    CreateControls();

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
    event.Skip();
}

void MainFrame::OnMove(wxMoveEvent& event)
{
    event.Skip();
}

void MainFrame::OnPrefsClicked(wxCommandEvent &event)
{
    //
}

void MainFrame::OnExitClicked(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAboutClicked(wxCommandEvent& event)
{
    //
}
