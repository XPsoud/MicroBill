#include "dlgaddeditestimitem.h"

#include <wx/statline.h>

DlgAddEditEstimItem::DlgAddEditEstimItem(wxWindow *parent) : wxDialog(parent, -1, _("New estimate entry"))
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgAddEditEstimItem\" object\n"));
#endif // __WXDEBUG__

    CreateControls();

    CenterOnParent();

    ConnectControls();
}

DlgAddEditEstimItem::~DlgAddEditEstimItem()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgAddEditEstimItem\" object\n"));
#endif // __WXDEBUG__
}

void DlgAddEditEstimItem::CreateControls()
{
    wxBoxSizer *szrMain, *hszr;
    wxStaticText *label;

    szrMain=new wxBoxSizer(wxVERTICAL);
        label=new wxStaticText(this, wxID_STATIC, _("Description:"));
        szrMain->Add(label, 0, wxLEFT|wxRIGHT|wxTOP, 5);
        szrMain->AddSpacer(1);
        m_txtDescr=new wxTextCtrl(this, -1, _T(""));
        szrMain->Add(m_txtDescr, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);

        label=new wxStaticText(this, wxID_STATIC, _("Comments:"));
        szrMain->Add(label, 0, wxLEFT|wxRIGHT|wxTOP, 5);
        szrMain->AddSpacer(1);
        m_txtComments=new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE);
        szrMain->Add(m_txtComments, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);

        hszr=new wxBoxSizer(wxHORIZONTAL);
            label=new wxStaticText(this, wxID_STATIC, _("Unit price:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            m_spdUnitPrice=new wxSpinCtrlDouble(this, -1, _T("0.00"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS|wxALIGN_RIGHT, -25000, 25000, 0);
                m_spdUnitPrice->SetDigits(2);
            hszr->Add(m_spdUnitPrice, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

            label=new wxStaticText(this, wxID_STATIC, _("Quantity:"));
            hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            m_spcQuantity=new wxSpinCtrl(this, -1, _T("1"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS|wxALIGN_RIGHT, 0, 100, 1);
            hszr->Add(m_spcQuantity, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
            label=new wxStaticText(this, wxID_STATIC, _("(let the quantity on zero to not\nshow any price on the estimate)"));
            hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
        szrMain->Add(hszr, 0, wxALL|wxEXPAND, 5);

        szrMain->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxALL|wxEXPAND, 5);

    SetSizer(szrMain);

    SetDefaultItem(FindWindowById(wxID_OK));

    szrMain->SetSizeHints(this);
}

void DlgAddEditEstimItem::ConnectControls()
{
    Bind(wxEVT_BUTTON, &DlgAddEditEstimItem::OnBtnValidClicked, this, wxID_OK);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimItem::OnUpdateUI_BtnValid, this, wxID_OK);
}

void DlgAddEditEstimItem::OnUpdateUI_BtnValid(wxUpdateUIEvent& event)
{
    event.Enable(!m_txtDescr->IsEmpty());
}

void DlgAddEditEstimItem::OnBtnValidClicked(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void DlgAddEditEstimItem::SetItemDatas(const wxString& descr, const wxString& comments, double unitPrice, int quantity)
{
    SetTitle(_("Edit an estimate entry"));
    m_txtDescr->ChangeValue(descr);
    m_txtComments->ChangeValue(comments);
    m_spdUnitPrice->SetValue(unitPrice);
    m_spcQuantity->SetValue(quantity);
}
