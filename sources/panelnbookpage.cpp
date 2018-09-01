#include "panelnbookpage.h"

PanelNBookPage::PanelNBookPage(wxWindow* parent, int type) : wxPanel(parent, -1),
    m_SettingsMngr(SettingsManager::Get())
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"PanelNBookPage\" object of type %d\n"), type);
#endif // __WXDEBUG__
    m_iType=PNBP_TYPE_UNKNOWN;
    if ((type>PNBP_TYPE_UNKNOWN) && (type<PNBP_TYPE_COUNT))
        m_iType=type;

    CreateControls();

    ConnectControls();
}

PanelNBookPage::~PanelNBookPage()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"PanelNBookPage\" object of type %d\n"), m_iType);
#endif // __WXDEBUG__
}

void PanelNBookPage::CreateControls()
{
    wxBoxSizer *szr=new wxBoxSizer(wxVERTICAL);
#ifdef __WXMAC__
        wxStaticBoxSizer *stbszr=new wxStaticBoxSizer(wxVERTICAL, this, _T(" "));
        szr->Add(stbszr, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
#else
        wxStaticBoxSizer *stbszr=new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
        szr->Add(stbszr, 1, wxALL|wxEXPAND, 5);
#endif
        m_stbMain=stbszr->GetStaticBox();

    SetSizer(szr);
    m_szrMain=stbszr;
}

void PanelNBookPage::ConnectControls()
{
    //
}
