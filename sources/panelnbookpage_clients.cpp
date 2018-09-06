#include "panelnbookpage_clients.h"

#include "main.h"
#include "dlgaddeditclient.h"

const wxString PanelNBookPage_Clients::m_sColItem[CI_COLS_COUNT] = {
        _T("Company"), _T("LastName"), _T("FirstName"), _T("Email"), _T("Phone"), _T("Cell")};

PanelNBookPage_Clients::PanelNBookPage_Clients(wxWindow* parent) :
    PanelNBookPage(parent, PNBP_TYPE_CLIENTS)
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"PanelNBookPage_Clients\" object\n"));
#endif // __WXDEBUG__
    CreateControls();
    for (int i=0; i<3; ++i) m_bColOrderAsc[i]=false;
    RefreshClientsList();
    ConnectControls();
}

PanelNBookPage_Clients::~PanelNBookPage_Clients()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"PanelNBookPage_Clients\" object\n"));
#endif // __WXDEBUG__
}

void PanelNBookPage_Clients::CreateControls()
{
    wxBoxSizer *hszr, *vszr;
    hszr=new wxBoxSizer(wxHORIZONTAL);
        hszr->Add(new wxStaticText(this, wxID_STATIC, _("Search filter:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(5);
        m_txtFilter=new wxTextCtrl(this, -1, wxEmptyString);
        hszr->Add(m_txtFilter, 1, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        hszr->Add(new wxStaticText(this, wxID_STATIC, _("Search field:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
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
    GetMainSizer()->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    m_lblResults=new wxStaticText(this, wxID_STATIC, _("Result:"));
    GetMainSizer()->Add(m_lblResults, 0, wxLEFT|wxRIGHT|wxTOP, 5);

    GetMainSizer()->AddSpacer(1);

    hszr=new wxBoxSizer(wxHORIZONTAL);
        m_lstClients=new wxListView(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES);
        hszr->Add(m_lstClients, 1, wxALL|wxEXPAND, 0);
            m_lstClients->AppendColumn(_("Company"));
            m_lstClients->AppendColumn(_("First name"));
            m_lstClients->AppendColumn(_("Last name"));
            m_lstClients->AppendColumn(_("Email"));
            m_lstClients->AppendColumn(_("Phone number"));
            m_lstClients->AppendColumn(_("Cell. Number"));
        hszr->AddSpacer(5);
        vszr=new wxBoxSizer(wxVERTICAL);
            m_btnAddClient=new wxButton(this, wxID_ADD, wxGetStockLabel(wxID_ADD));
            vszr->Add(m_btnAddClient, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnDelClient=new wxButton(this, wxID_DELETE, wxGetStockLabel(wxID_DELETE));
            vszr->Add(m_btnDelClient, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnEditClient=new wxButton(this, wxID_EDIT, _("Modify"));
            vszr->Add(m_btnEditClient, 0, wxALL|wxEXPAND, 0);
        hszr->Add(vszr, 0, wxALL|wxEXPAND, 0);
    GetMainSizer()->Add(hszr, 1, wxLEFT|wxRIGHT|wxEXPAND, 5);

    GetMainSizer()->AddSpacer(5);

    // Get minimal columns widths
    for (int i=0; i<CI_COLS_COUNT; ++i)
    {
        m_lstClients->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
        m_iMinWidth[i] = m_lstClients->GetColumnWidth(i);
    }
}

void PanelNBookPage_Clients::ConnectControls()
{
    m_txtFilter->Bind(wxEVT_TEXT,&PanelNBookPage_Clients::OnTxtFilterChanged, this);
    m_cmbFilterField->Bind(wxEVT_CHOICE, &PanelNBookPage_Clients::OnCmbFilterFieldChanged, this);
    m_btnAddClient->Bind(wxEVT_BUTTON, &PanelNBookPage_Clients::OnBtnAddClientClicked, this);
    m_btnDelClient->Bind(wxEVT_BUTTON, &PanelNBookPage_Clients::OnBtnDelClientClicked, this);
    m_btnEditClient->Bind(wxEVT_BUTTON, &PanelNBookPage_Clients::OnBtnEditClientClicked, this);
    m_lstClients->Bind(wxEVT_LIST_ITEM_ACTIVATED, &PanelNBookPage_Clients::OnLstClientItemActivated, this);
    m_lstClients->Bind(wxEVT_LIST_COL_CLICK, &PanelNBookPage_Clients::OnLstClientsColumnClicked, this);

    m_btnDelClient->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Clients::OnUpdateUI_Btn_Add_Del, this);
    m_btnEditClient->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Clients::OnUpdateUI_Btn_Add_Del, this);
}

void PanelNBookPage_Clients::RefreshClientsList()
{
    int iCount=m_DatasMngr.GetClientsCount();
    long lSel=m_lstClients->GetFirstSelected();
    Client* selItem;
    if (lSel==wxNOT_FOUND)
        selItem=NULL;
    else
        selItem=(Client*)m_lstClients->GetItemData(lSel);
    m_lstClients->DeleteAllItems();
    wxString sFilter=m_txtFilter->GetValue();
    bool bDoFilter=(!sFilter.IsEmpty());
    int iField=m_cmbFilterField->GetSelection()-1;
    if (iField==CATTR_ADDR1) iField=CATTR_ADDRESS;

    for (int i=0; i<iCount; ++i)
    {
        Client* item=m_DatasMngr.GetClient(i);
        if (bDoFilter)
        {
            if (item->Matches(sFilter, iField))
            {
                InsertClientItemToList(item, wxID_ANY, (item==selItem));
            }
        }
        else
        {
            InsertClientItemToList(item, wxID_ANY, (item==selItem));
        }
    }
    UpdateColumnsWidths();
}

long PanelNBookPage_Clients::InsertClientItemToList(Client* item, long before, bool select, bool updateWidths)
{
    if (item==NULL) return wxNOT_FOUND;

    long lBefore=(before==wxID_ANY?m_lstClients->GetItemCount():before);
    long lRes=m_lstClients->InsertItem(lBefore, wxEmptyString);

    wxString sItem;

    for (int i=0; i<CI_COLS_COUNT; ++i)
    {
        sItem = item->GetAttribute(m_sColItem[i]);
        m_lstClients->SetItem(lRes, i, sItem);
        if ((!sItem.IsEmpty()) && updateWidths)
        {
            m_lstClients->SetColumnWidth(i, wxLIST_AUTOSIZE);
            int iWdth = m_lstClients->GetColumnWidth(i);
            if (iWdth < m_iMinWidth[i])
                m_lstClients->SetColumnWidth(i, m_iMinWidth[i]);
        }
    }

    m_lstClients->SetItemPtrData(lRes, wxUIntPtr(item));

    if (select)
    {
        m_lstClients->EnsureVisible(lRes);
        m_lstClients->Select(lRes);
    }

    return lRes;
}

void PanelNBookPage_Clients::UpdateClientItem(long lItem)
{
    if (lItem==wxNOT_FOUND) return;
    Client* item=(Client*)m_lstClients->GetItemData(lItem);
    if (item==NULL) return;
    for (int i=0; i<CI_COLS_COUNT; ++i)
    {
        m_lstClients->SetItem(lItem, i, item->GetAttribute(m_sColItem[i]));
    }
}

void PanelNBookPage_Clients::UpdateColumnsWidths()
{
    int iCount = m_lstClients->GetItemCount();
    if (iCount == 0)
        return;
    bool bCol[CI_COLS_COUNT];
    for (int i=0; i<CI_COLS_COUNT; ++i)
        bCol[i] = false;

    for(int iCol = 0; iCol < CI_COLS_COUNT; ++iCol)
    {
        for(int iItem =0; iItem < iCount; ++iItem)
        {
            if (!bCol[iCol])
            {
                if (m_lstClients->GetItemText(iItem, iCol) != wxEmptyString)
                    bCol[iCol] = true;
            }
            else
            {
                iItem = iCount;
            }
        }
        if (bCol[iCol])
        {
            m_lstClients->SetColumnWidth(iCol, wxLIST_AUTOSIZE);
            int iWdth = m_lstClients->GetColumnWidth(iCol);
            if (iWdth < m_iMinWidth[iCol])
                m_lstClients->SetColumnWidth(iCol, wxLIST_AUTOSIZE_USEHEADER);
        }
        else
        {
            m_lstClients->SetColumnWidth(iCol, wxLIST_AUTOSIZE_USEHEADER);
        }
    }

}

void PanelNBookPage_Clients::OnTxtFilterChanged(wxCommandEvent& event)
{
    RefreshClientsList();
}

void PanelNBookPage_Clients::OnCmbFilterFieldChanged(wxCommandEvent& event)
{
    RefreshClientsList();
}

void PanelNBookPage_Clients::OnUpdateUI_Btn_Add_Del(wxUpdateUIEvent& event)
{
    event.Enable(m_lstClients->GetSelectedItemCount()!=0);
}

void PanelNBookPage_Clients::OnBtnAddClientClicked(wxCommandEvent& event)
{
    DlgAddEditClient dlg(wxTheApp->GetTopWindow());
    if (dlg.ShowModal()==wxID_OK)
    {
        Client* item=m_DatasMngr.AddNewClient();
        dlg.SaveToClientItem(item);
        InsertClientItemToList(item, wxID_ANY, true, true);
    }
}

void PanelNBookPage_Clients::OnBtnDelClientClicked(wxCommandEvent& event)
{
    long lRes=m_lstClients->GetFirstSelected();
    if (lRes==wxNOT_FOUND) return;
    Client* item=(Client*)m_lstClients->GetItemData(lRes);
    if (m_DatasMngr.IsClientUsed(item))
    {
        wxString sMsg=_("The selected client is referenced by another element.\nIt Can't actually be removed from the database.\n\nRemove the elements that has a reference to it first.");
        wxMessageBox(sMsg, _("Impossible operation"), wxICON_EXCLAMATION|wxCENTER|wxOK);
        return;
    }
    if (wxMessageBox(_("Remove the selected client?"), _("Confirmation"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES)
        return;
    if (!m_DatasMngr.RemoveClient(item))
    {
        wxMessageBox(_("An error occurred while removing the selected client!"), _("Error"), wxICON_ERROR|wxCENTER|wxOK);
        return;
    }
    m_lstClients->DeleteItem(lRes);
}

void PanelNBookPage_Clients::OnBtnEditClientClicked(wxCommandEvent& event)
{
    long lRes=m_lstClients->GetFirstSelected();
    if (lRes==wxNOT_FOUND) return;
    Client* item=(Client*)m_lstClients->GetItemData(lRes);
    DlgAddEditClient dlg(wxTheApp->GetTopWindow(), item);
    if (dlg.ShowModal()==wxID_OK)
        UpdateClientItem(lRes);
}

void PanelNBookPage_Clients::OnLstClientItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, wxID_EDIT);
    OnBtnEditClientClicked(evt);
}

void PanelNBookPage_Clients::OnLstClientsColumnClicked(wxListEvent& event)
{
    int iCol=event.GetColumn();
    // Sorting only allowed for the 3 first columns
    if ((iCol<0) || (iCol>2)) return;
    m_bColOrderAsc[iCol]=!m_bColOrderAsc[iCol];
    m_DatasMngr.SortClients(iCol, m_bColOrderAsc[iCol]);
    RefreshClientsList();
}

void PanelNBookPage_Clients::RefreshDatasLists()
{
    RefreshClientsList();
}
