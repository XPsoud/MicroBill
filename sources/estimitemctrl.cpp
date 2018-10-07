#include "estimitemctrl.h"

#include "settingsmanager.h"

wxFont EstimItemCtrl::m_fntDescr=wxNullFont;
wxFont EstimItemCtrl::m_fntFixed=wxNullFont;
wxColour EstimItemCtrl::m_colHiglight=wxNullColour;
wxColour EstimItemCtrl::m_colNormal=wxNullColour;

EstimItemCtrl::EstimItemCtrl(wxWindow *parent) : wxPanel(parent),
    m_options(SettingsManager::Get())
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"EstimItemCtrl\" object\n"));
#endif // __WXDEBUG__

    if (!m_fntDescr.IsOk())
    {
        m_fntDescr=GetFont();
        m_fntDescr.SetWeight(wxFONTWEIGHT_BOLD);
        m_fntFixed=GetFont();
#ifdef __WXGTK__
        m_fntFixed.SetFaceName(_T("Courier 10 Pitch"));
#else
        m_fntFixed.SetFaceName(_T("Courier New"));
#endif // __WXGTK__
        m_fntFixed.SetWeight(wxFONTWEIGHT_BOLD);
        m_colHiglight=wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        m_colNormal=wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    }

    m_sDescr=wxEmptyString;
    m_sComments=wxEmptyString;
    m_dUnitPrice=0.;
    m_iQuantity=0;
    m_iIndex=-1;

    m_bBackgroundChanged=false;

    CreateControls();

    ConnectControls();
}

EstimItemCtrl::~EstimItemCtrl()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"EstimItemCtrl\" object\n"));
#endif // __WXDEBUG__
}

void EstimItemCtrl::SetDatas(const wxString& descr, const wxString& comm, double unitPrice, int quantity)
{
    m_sDescr=descr;
    m_sComments=comm;
    m_dUnitPrice=unitPrice;
    m_iQuantity=quantity;

    Update();
}

void EstimItemCtrl::CreateControls()
{
    SetBackgroundColour(m_colNormal);

    wxBoxSizer *szrMain=new wxBoxSizer(wxVERTICAL);
        wxBoxSizer *hszr=new wxBoxSizer(wxHORIZONTAL);
            m_btnEdit=new wxButton(this, wxID_EDIT, wxGetStockLabel(wxID_EDIT, wxSTOCK_FOR_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            hszr->Add(m_btnEdit, 0, wxALL|wxEXPAND, 0);
            m_btnDelete=new wxButton(this, wxID_DELETE, wxGetStockLabel(wxID_DELETE, wxSTOCK_FOR_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            hszr->Add(m_btnDelete, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
            m_btnMoveUp=new wxButton(this, wxID_UP, _("Move up"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            hszr->Add(m_btnMoveUp, 0, wxALL|wxEXPAND, 0);
            m_btnMoveDown=new wxButton(this, wxID_DOWN, _("Move down"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            hszr->Add(m_btnMoveDown, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
        szrMain->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP, 5);
        m_lblDescr=new wxStaticText(this, wxID_STATIC, _T("Description"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
            m_lblDescr->SetFont(m_fntDescr);
        szrMain->Add(m_lblDescr, 0, wxALL, 5);
        m_lblComments=new wxStaticText(this, wxID_STATIC, _T("Comments"));
        szrMain->Add(m_lblComments, 0, wxLEFT, 20);
        m_lblPrice=new wxStaticText(this, wxID_STATIC, _T("Prices && Quantity"));
            m_lblPrice->SetFont(m_fntFixed);
        szrMain->AddSpacer(1);
        szrMain->Add(m_lblPrice, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 5);
    SetSizer(szrMain);
}

void EstimItemCtrl::ConnectControls()
{
    m_lblDescr->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
    m_lblComments->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
    m_lblPrice->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);

    Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &EstimItemCtrl::OnMouseLeave, this);

    wxSizer *szr=GetSizer();
    int iCount=szr->GetItemCount();
    for (int i=0; i<iCount; ++i)
    {
        wxSizerItem *sitm=szr->GetItem(i);
        if (sitm->IsWindow())
        {
            sitm->GetWindow()->Bind(wxEVT_LEAVE_WINDOW, &EstimItemCtrl::OnMouseLeave, this);
            sitm->GetWindow()->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
        }
    }
    m_btnEdit->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
    m_btnDelete->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
    m_btnMoveUp->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);
    m_btnMoveDown->Bind(wxEVT_ENTER_WINDOW, &EstimItemCtrl::OnMouseEnter, this);

    m_btnEdit->Bind(wxEVT_LEAVE_WINDOW, &EstimItemCtrl::OnMouseLeave, this);
    m_btnDelete->Bind(wxEVT_LEAVE_WINDOW, &EstimItemCtrl::OnMouseLeave, this);
    m_btnMoveUp->Bind(wxEVT_LEAVE_WINDOW, &EstimItemCtrl::OnMouseLeave, this);
    m_btnMoveDown->Bind(wxEVT_LEAVE_WINDOW, &EstimItemCtrl::OnMouseLeave, this);
}

void EstimItemCtrl::Update()
{
    m_lblDescr->SetLabel(m_sDescr);
    m_lblComments->SetLabel(m_sComments);
    wxString sPrice = wxEmptyString;
    sPrice << _("Unit price:") << _T(" ") << m_options.GetFormatedMoneyValue(m_dUnitPrice, _T("%6.02f"));
    sPrice << _T("      ") << wxString::Format(_("Q. : %3d"), m_iQuantity);
    sPrice << _T("      ") << _("Price:") << _T(" ") << m_options.GetFormatedMoneyValue(double(m_dUnitPrice*m_iQuantity), _T("%9.02f"));

    m_lblPrice->SetLabel(sPrice);
    GetSizer()->SetSizeHints(this);
}

void EstimItemCtrl::OnMouseEnter(wxMouseEvent& event)
{
    if (!m_bBackgroundChanged)
    {
        SetBackgroundColour(m_colHiglight);
        Refresh();
        m_bBackgroundChanged=true;
    }
    event.Skip();
}

void EstimItemCtrl::OnMouseLeave(wxMouseEvent& event)
{
    wxPoint pt;
    wxWindow *w=wxFindWindowAtPointer(pt);
    if (!IsDescendant(w))
    {
        SetBackgroundColour(m_colNormal);
        m_bBackgroundChanged=false;
        Refresh();
    }
    event.Skip();
}
