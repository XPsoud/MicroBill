#include "dlgselectclient.h"

#include <wx/statline.h>

#include "client.h"
#include "datasmanager.h"

DlgSelectClient::DlgSelectClient(wxWindow *parent, Client* item) : wxDialog(parent, -1, _("Selecting a client"))
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgSelectClient\" object\n"));
#endif // __WXDEBUG__
    m_Client=item;

    CreateControls();

    CenterOnParent();
    UpdateClientsList();

    ConnectControls();
}

DlgSelectClient::~DlgSelectClient()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgSelectClient\" object\n"));
#endif // __WXDEBUG__
}

void DlgSelectClient::CreateControls()
{
    wxBoxSizer *hszr, *szrMain;
    szrMain=new wxBoxSizer(wxVERTICAL);
        hszr=new wxBoxSizer(wxHORIZONTAL);
            hszr->Add(new wxStaticText(this, wxID_STATIC, _("Filter:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            hszr->AddSpacer(5);
            m_txtFilter=new wxTextCtrl(this, -1, wxEmptyString);
            hszr->Add(m_txtFilter, 1, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            hszr->AddSpacer(10);
            hszr->Add(new wxStaticText(this, wxID_STATIC, _("Field:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            hszr->AddSpacer(5);
            m_cmbFilterField=new wxChoice(this, -1);
                m_cmbFilterField->Append(_("All fields"));
                m_cmbFilterField->Append(_("Company"));
                m_cmbFilterField->Append(_("Last name"));
                m_cmbFilterField->Append(_("First name"));
                m_cmbFilterField->Append(_("Email"));
                m_cmbFilterField->Append(_("Phone"));
                m_cmbFilterField->Append(_("Address"));
                m_cmbFilterField->SetSelection(0);
            hszr->Add(m_cmbFilterField, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        szrMain->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

        m_lblResults=new wxStaticText(this, wxID_STATIC, _("Results:"));
        szrMain->Add(m_lblResults, 0, wxLEFT|wxRIGHT|wxTOP, 5);

        szrMain->AddSpacer(1);

        m_lstClients=new wxListBox(this, -1, wxDefaultPosition, wxSize(-1, 150));
        szrMain->Add(m_lstClients, 1, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);

        szrMain->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxALL|wxEXPAND, 5);

    SetSizer(szrMain);

    szrMain->SetSizeHints(this);
}

void DlgSelectClient::ConnectControls()
{
    m_txtFilter->Bind(wxEVT_TEXT, &DlgSelectClient::OnTxtFilterChanged, this);
    m_cmbFilterField->Bind(wxEVT_CHOICE, &DlgSelectClient::OnCmbFilterFieldChanged, this);
    m_lstClients->Bind(wxEVT_LISTBOX_DCLICK, &DlgSelectClient::OnLstClientsDblClick, this);

    Bind(wxEVT_UPDATE_UI, &DlgSelectClient::OnUpdateUI_BtnValid, this, wxID_OK);
}

void DlgSelectClient::OnUpdateUI_BtnValid(wxUpdateUIEvent& event)
{
    long lItem=m_lstClients->GetSelection();
    if (lItem==wxNOT_FOUND)
    {
        m_Client=NULL;
        event.Enable(false);
    }
    else
    {
        m_Client=(Client*)m_lstClients->GetClientData(lItem);
        event.Enable(true);
    }
}

void DlgSelectClient::UpdateClientsList()
{
    wxArrayString arsClients;
    arsClients.Clear();
    DatasManager& datas=DatasManager::Get();

    wxString sFilter=m_txtFilter->GetValue();
    bool bDoFilter=(!sFilter.IsEmpty());
    int iField=m_cmbFilterField->GetSelection()-1;
    if (iField==CATTR_ADDR1) iField=CATTR_ADDRESS;

    int iCount=DatasManager::Get().GetClientsCount();
    if (iCount>0)
    {
        Client** clients=new Client*[iCount];
        for (int i=0; i<iCount; ++i)
        {
            if (bDoFilter)
            {
                if (datas.GetClient(i)->Matches(sFilter, iField))
                {
                    clients[i]=datas.GetClient(i);
                    arsClients.Add(datas.GetClientString(clients[i]));
                }
            }
            else
            {
                clients[i]=datas.GetClient(i);
                arsClients.Add(datas.GetClientString(clients[i]));
            }
        }
        m_lstClients->Set(arsClients, (void**)clients);
        delete [] clients;
    }
    else
        m_lstClients->Set(arsClients);

    // Re-selected the last selected item if it is always in the list
    for (size_t i=0; i<m_lstClients->GetCount(); ++i)
    {
        Client* item=(Client*)m_lstClients->GetClientData(i);
        if (item==m_Client)
        {
            m_lstClients->SetSelection(i);
            return;
        }
    }
    // Last selected item isn't in the list
    m_Client=NULL;
}

void DlgSelectClient::OnTxtFilterChanged(wxCommandEvent& event)
{
    UpdateClientsList();
}

void DlgSelectClient::OnCmbFilterFieldChanged(wxCommandEvent& event)
{
    UpdateClientsList();
}

void DlgSelectClient::OnLstClientsDblClick(wxCommandEvent& event)
{
    long lItem=m_lstClients->GetSelection();
    if (lItem==wxNOT_FOUND)
        return;

    Client* item=(Client*)m_lstClients->GetClientData(lItem);
    if (item!=NULL)
    {
        m_Client=item;
        EndModal(wxID_OK);
    }
}
