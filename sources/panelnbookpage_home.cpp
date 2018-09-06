#include "panelnbookpage_home.h"

#include "main.h"

#include "microbill.png.h"

PanelNBookPage_Home::PanelNBookPage_Home(wxWindow* parent) :
    PanelNBookPage(parent, PNBP_TYPE_HOME)
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"PanelNBookPage_Home\" object\n"));
#endif // __WXDEBUG__
    CreateControls();
    ConnectControls();
}

PanelNBookPage_Home::~PanelNBookPage_Home()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"PanelNBookPage_Home\" object\n"));
#endif // __WXDEBUG__
}

void PanelNBookPage_Home::CreateControls()
{
    wxString sTxt=wxGetApp().GetBuildInfos(true);

    wxSizer *szrPage=GetMainSizer();
#ifdef __WXDEBUG__
    m_btnStart=new wxButton(this, -1, _T("Start test"));
    szrPage->Add(m_btnStart, 0, wxALL, 5);
#endif // __WXDEBUG__

    szrPage->AddStretchSpacer(1);

    wxStaticBitmap *stBmp=new wxStaticBitmap(this, wxID_STATIC, wxGet_MicroBill_png_Bitmap());
    szrPage->Add(stBmp, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER, 5);

    wxStaticText *label = new wxStaticText(this, wxID_STATIC, _("Your micro entreprise billing software"));
    wxFont fnt = label->GetFont().MakeLarger().MakeLarger().MakeLarger();
    label->SetFont(fnt);
    szrPage->Add(label, 0, wxALIGN_CENTER, 0);

    szrPage->AddStretchSpacer(1);
}

void PanelNBookPage_Home::ConnectControls()
{
#ifdef __WXDEBUG__
    m_btnStart->Bind(wxEVT_BUTTON, &PanelNBookPage_Home::OnDoStartTest, this);
#endif // __WXDEBUG__
}

#ifdef __WXDEBUG__
void PanelNBookPage_Home::OnDoStartTest(wxCommandEvent& event)
{
    wxMessageBox(_T("Test function terminated..."));
}
#endif // __WXDEBUG__
