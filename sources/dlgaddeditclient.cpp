#include "dlgaddeditclient.h"

#include <wx/statline.h>

#include "client.h"

DlgAddEditClient::DlgAddEditClient(wxWindow *parent, Client* item) : wxDialog(parent, -1, _T("Add-Edit client"))
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgAddEditClient\" object\n"));
#endif // __WXDEBUG__
    m_Client=item;
    if (item==NULL)
        SetTitle(_("Creating a new client"));
    else
        SetTitle(_("Editing a client"));
    CreateControls();

    CenterOnParent();

    ConnectControls();
}

DlgAddEditClient::~DlgAddEditClient()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgAddEditClient\" object\n"));
#endif // __WXDEBUG__
}

void DlgAddEditClient::CreateControls()
{
    wxBoxSizer *szrMain, *hszr;
    wxFlexGridSizer *grdszr;
    wxStaticText *label;

    szrMain=new wxBoxSizer(wxVERTICAL);
        hszr=new wxBoxSizer(wxHORIZONTAL);
            label=new wxStaticText(this, wxID_STATIC, _("Company / Association:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            m_txtCompany=new wxTextCtrl(this, -1, _T(""));
            hszr->Add(m_txtCompany, 1, wxLEFT, 5);
        szrMain->Add(hszr, 0, wxALL|wxEXPAND, 5);

        grdszr=new wxFlexGridSizer(4, wxSize(5,5));
            label=new wxStaticText(this, wxID_STATIC, _("Last name:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtLastName=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtLastName, 1, wxEXPAND);
            label=new wxStaticText(this, wxID_STATIC, _("First name:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtFirstName=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtFirstName, 1, wxEXPAND);

            label=new wxStaticText(this, wxID_STATIC, _("Email:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtEmail=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtEmail, 1, wxEXPAND);
            label=new wxStaticText(this, wxID_STATIC, _("Phone number:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtPhone=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtPhone, 1, wxEXPAND);

            label=new wxStaticText(this, wxID_STATIC, _T(" "));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            label=new wxStaticText(this, wxID_STATIC, _T(" "));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            label=new wxStaticText(this, wxID_STATIC, _("Cell number:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtCell=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtCell, 1, wxEXPAND);
        grdszr->AddGrowableCol(1, 1);
        grdszr->AddGrowableCol(3, 1);
        szrMain->Add(grdszr, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

        grdszr=new wxFlexGridSizer(2, wxSize(5,5));
            label=new wxStaticText(this, wxID_STATIC, _("Address #1:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtAddr1=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtAddr1, 1, wxEXPAND);

            label=new wxStaticText(this, wxID_STATIC, _("Address #2:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);
            m_txtAddr2=new wxTextCtrl(this, -1, _T(""));
            grdszr->Add(m_txtAddr2, 1, wxEXPAND);

            label=new wxStaticText(this, wxID_STATIC, _("Zip code:"));
            grdszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 0);

            hszr=new wxBoxSizer(wxHORIZONTAL);
                m_txtZipCode=new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxSize(50, -1));
                hszr->Add(m_txtZipCode, 1, wxEXPAND);
                label=new wxStaticText(this, wxID_STATIC, _("City:"));
                hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                m_txtCity=new wxTextCtrl(this, -1);
                hszr->Add(m_txtCity, 2, wxLEFT|wxEXPAND, 5);
                label=new wxStaticText(this, wxID_STATIC, _("Country:"));
                hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                m_txtCountry=new wxTextCtrl(this, -1);
                hszr->Add(m_txtCountry, 2, wxLEFT|wxEXPAND, 5);
            grdszr->Add(hszr, 1, wxEXPAND);
        grdszr->AddGrowableCol(1, 1);
        szrMain->Add(grdszr, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

        szrMain->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxALL|wxEXPAND, 5);

    SetSizer(szrMain);

    if (m_Client!=NULL)
    {
        m_txtCompany->ChangeValue(m_Client->GetAttribute(_T("Company")));
        m_txtLastName->ChangeValue(m_Client->GetAttribute(_T("LastName")));
        m_txtFirstName->ChangeValue(m_Client->GetAttribute(_T("FirstName")));
        m_txtEmail->ChangeValue(m_Client->GetAttribute(_T("Email")));
        m_txtPhone->ChangeValue(m_Client->GetAttribute(_T("Phone")));
        m_txtCell->ChangeValue(m_Client->GetAttribute(_T("Cell")));
        m_txtAddr1->ChangeValue(m_Client->GetAttribute(_T("Addr1")));
        m_txtAddr2->ChangeValue(m_Client->GetAttribute(_T("Addr2")));
        m_txtZipCode->ChangeValue(m_Client->GetAttribute(_T("ZipCode")));
        m_txtCity->ChangeValue(m_Client->GetAttribute(_T("City")));
        m_txtCountry->ChangeValue(m_Client->GetAttribute(_T("Country")));
    }

    szrMain->SetSizeHints(this);
}

void DlgAddEditClient::ConnectControls()
{
    Bind(wxEVT_BUTTON, &DlgAddEditClient::OnBtnValidClicked, this, wxID_OK);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditClient::OnUpdateUI_BtnValid, this, wxID_OK);
}

void DlgAddEditClient::OnUpdateUI_BtnValid(wxUpdateUIEvent& event)
{
    bool bEnable=true;
    if (m_txtLastName->IsEmpty() && m_txtCompany->IsEmpty()) bEnable=false;
    //if ((m_txtEmail->IsEmpty()) && (m_txtPhone->IsEmpty()) && (m_txtCell->IsEmpty())) bEnable=false;
    /* TODO (Xaviou#1#): Make a real check fo the email and phone number formats */
    event.Enable(bEnable);
}

void DlgAddEditClient::OnBtnValidClicked(wxCommandEvent& event)
{
    if (m_Client!=NULL)
        SaveToClientItem(m_Client);
    EndModal(wxID_OK);
}

void DlgAddEditClient::SaveToClientItem(Client* item)
{
    // Set or remove each attribute
    if (!m_txtCompany->IsEmpty())
        item->AddAttribute(_T("Company"), m_txtCompany->GetValue());
    else
        item->DeleteAttribute(_T("Company"));
    if (!m_txtLastName->IsEmpty())
        item->AddAttribute(_T("LastName"), m_txtLastName->GetValue());
    else
        item->DeleteAttribute(_T("LastName"));
    if (!m_txtFirstName->IsEmpty())
        item->AddAttribute(_T("FirstName"), m_txtFirstName->GetValue());
    else
        item->DeleteAttribute(_T("FirstName"));
    if (!m_txtEmail->IsEmpty())
        item->AddAttribute(_T("Email"), m_txtEmail->GetValue());
    else
        item->DeleteAttribute(_T("Email"));
    if (!m_txtPhone->IsEmpty())
        item->AddAttribute(_T("Phone"), m_txtPhone->GetValue());
    else
        item->DeleteAttribute(_T("Phone"));
    if (!m_txtCell->IsEmpty())
        item->AddAttribute(_T("Cell"), m_txtCell->GetValue());
    else
        item->DeleteAttribute(_T("Cell"));
    if (!m_txtAddr1->IsEmpty())
        item->AddAttribute(_T("Addr1"), m_txtAddr1->GetValue());
    else
        item->DeleteAttribute(_T("Addr1"));
    if (!m_txtAddr2->IsEmpty())
        item->AddAttribute(_T("Addr2"), m_txtAddr2->GetValue());
    else
        item->DeleteAttribute(_T("Addr2"));
    if (!m_txtZipCode->IsEmpty())
        item->AddAttribute(_T("ZipCode"), m_txtZipCode->GetValue());
    else
        item->DeleteAttribute(_T("ZipCode"));
    if (!m_txtCity->IsEmpty())
        item->AddAttribute(_T("City"), m_txtCity->GetValue());
    else
        item->DeleteAttribute(_T("City"));
    if (!m_txtCountry->IsEmpty())
        item->AddAttribute(_T("Country"), m_txtCountry->GetValue());
    else
        item->DeleteAttribute(_T("Country"));
}
