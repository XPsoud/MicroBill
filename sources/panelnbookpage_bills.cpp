#include "panelnbookpage_bills.h"
// TODO (Xaviou#1#): Check this file for i18n
#include "main.h"
#include "dlgaddeditbill.h"

#include <wx/mimetype.h>

enum
{
    BI_COL_NUMBER = BSC_NUMBER,
    BI_COL_DATE_CREATION = BSC_CREAT_DATE,
    BI_COL_CLIENT = BSC_CLIENT,
    BI_COL_VALUE = BSC_TOTAL,
    BI_COL_STATUS = BSC_STATUS,
    BI_COL_DATE_LIMIT = BSC_TERM_DATE
};

PanelNBookPage_Bills::PanelNBookPage_Bills(wxWindow* parent) :
    PanelNBookPage(parent, PNBP_TYPE_BILLS)
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"PanelNBookPage_Bills\" object\n"));
#endif // __WXDEBUG__
    for (int i=0; i<3; ++i) m_bColOrderAsc[i]=false;
    CreateControls();
    RefreshBillsList();
    ConnectControls();
}

PanelNBookPage_Bills::~PanelNBookPage_Bills()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"PanelNBookPage_Bills\" object\n"));
#endif // __WXDEBUG__
}

void PanelNBookPage_Bills::CreateControls()
{
    wxBoxSizer *hszr, *vszr;

    hszr=new wxBoxSizer(wxHORIZONTAL);
        hszr->Add(new wxStaticText(this, wxID_STATIC, _T("Affichage :")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(5);
        m_optDisplay[0]=new wxRadioButton(this, -1, _T("Toutes les factures"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
            m_optDisplay[0]->SetValue(true);
        hszr->Add(m_optDisplay[0], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        m_optDisplay[1]=new wxRadioButton(this, -1, _T("Factures payées"));
        hszr->Add(m_optDisplay[1], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        m_optDisplay[2]=new wxRadioButton(this, -1, _T("Factures non payées"));
        hszr->Add(m_optDisplay[2], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    GetMainSizer()->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    hszr=new wxBoxSizer(wxHORIZONTAL);
        m_lstBills=new wxListView(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES);
        hszr->Add(m_lstBills, 1, wxALL|wxEXPAND, 0);
            m_lstBills->AppendColumn(_T("No"), wxLIST_FORMAT_RIGHT);
            m_lstBills->AppendColumn(_T("Date"));
            m_lstBills->AppendColumn(_T("Client"));
            m_lstBills->AppendColumn(_T("Montant"), wxLIST_FORMAT_RIGHT);
            m_lstBills->AppendColumn(_T("Statut"), wxLIST_FORMAT_CENTER);
            m_lstBills->AppendColumn(_T("Date limite"));
        hszr->AddSpacer(5);
        vszr=new wxBoxSizer(wxVERTICAL);
            m_btnAdd=new wxButton(this, wxID_ADD, _T("Nouveau"));
            vszr->Add(m_btnAdd, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnDel=new wxButton(this, wxID_DELETE, _T("Supprimer"));
                m_btnDel->Disable();
            vszr->Add(m_btnDel, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnEdit=new wxButton(this, wxID_EDIT, _T("Modifier"));
                m_btnEdit->Disable();
            vszr->Add(m_btnEdit, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(15);
            m_btnSaveAs=new wxButton(this, wxID_SAVEAS, _T("Exporter"));
                m_btnSaveAs->Disable();
            vszr->Add(m_btnSaveAs, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnCopy=new wxButton(this, wxID_COPY, _T("Dupliquer"));
                m_btnCopy->Disable();
            vszr->Add(m_btnCopy, 0, wxALL|wxEXPAND, 0);
        hszr->Add(vszr, 0, wxALL|wxEXPAND, 0);
    GetMainSizer()->Add(hszr, 1, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    GetMainSizer()->AddSpacer(5);
}

void PanelNBookPage_Bills::ConnectControls()
{
    for (int i=0; i<3; ++i)
        m_optDisplay[i]->Connect(wxEVT_RADIOBUTTON, wxCommandEventHandler(PanelNBookPage_Bills::OnOptDisplayClicked), NULL, this);

    m_btnAdd->Connect(wxEVT_BUTTON, wxCommandEventHandler(PanelNBookPage_Bills::OnBtnAddClicked), NULL, this);
    m_btnDel->Connect(wxEVT_BUTTON, wxCommandEventHandler(PanelNBookPage_Bills::OnBtnDelClicked), NULL, this);
    m_btnEdit->Connect(wxEVT_BUTTON, wxCommandEventHandler(PanelNBookPage_Bills::OnBtnEditClicked), NULL, this);
    m_btnSaveAs->Connect(wxEVT_BUTTON, wxCommandEventHandler(PanelNBookPage_Bills::OnBtnSaveAsClicked), NULL, this);
    m_btnCopy->Connect(wxEVT_BUTTON, wxCommandEventHandler(PanelNBookPage_Bills::OnBtnCopyClicked), NULL, this);
    m_lstBills->Connect(wxEVT_LIST_ITEM_ACTIVATED, wxListEventHandler(PanelNBookPage_Bills::OnLstItemActivated), NULL, this);
    m_lstBills->Connect(wxEVT_LIST_COL_CLICK, wxListEventHandler(PanelNBookPage_Bills::OnLstBillsColumnClicked), NULL, this);

    m_btnDel->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PanelNBookPage_Bills::OnUpdateUI_ItemButtons), NULL, this);
    m_btnEdit->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PanelNBookPage_Bills::OnUpdateUI_ItemButtons), NULL, this);
    m_btnSaveAs->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PanelNBookPage_Bills::OnUpdateUI_ItemButtons), NULL, this);
    m_btnCopy->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(PanelNBookPage_Bills::OnUpdateUI_ItemButtons), NULL, this);
}

void PanelNBookPage_Bills::OnOptDisplayClicked(wxCommandEvent& event)
{
    RefreshBillsList();
}

void PanelNBookPage_Bills::OnBtnAddClicked(wxCommandEvent& event)
{
    DlgAddEditBill dlg(wxTheApp->GetTopWindow());
    dlg.SetBillNumber(m_DatasMngr.GetLastBillNumber()+1);
    if (dlg.ShowModal()==wxID_OK)
    {
        Bill* item=m_DatasMngr.AddNewBill();
        dlg.SaveToBillItem(item);
        InsertBillItemToList(item, wxID_ANY, true, true);
    }
}

void PanelNBookPage_Bills::OnBtnDelClicked(wxCommandEvent& event)
{
    long lRes=m_lstBills->GetFirstSelected();
    if (lRes==wxNOT_FOUND) return;
    Bill* item=(Bill*)m_lstBills->GetItemData(lRes);
    if (wxMessageBox(_T("Supprimer la facture sélectionnée ?"), _T("Confirmation"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES)
        return;
    if (!m_DatasMngr.RemoveBill(item))
    {
        wxMessageBox(_T("Une erreur est survenue lors de la suppression de la facture sélectionnée !"), _T("Erreur"), wxICON_ERROR|wxCENTER|wxOK);
        return;
    }
    m_lstBills->DeleteItem(lRes);
}

void PanelNBookPage_Bills::OnBtnEditClicked(wxCommandEvent& event)
{
    long lRes=m_lstBills->GetFirstSelected();
    if (lRes==wxNOT_FOUND) return;
    Bill* item=(Bill*)m_lstBills->GetItemData(lRes);
    DlgAddEditBill dlg(wxTheApp->GetTopWindow(), item);
    if (dlg.ShowModal()==wxID_OK)
        UpdateBillItem(lRes);
}

void PanelNBookPage_Bills::OnLstItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, wxID_EDIT);
    OnBtnEditClicked(evt);
}

void PanelNBookPage_Bills::OnLstBillsColumnClicked(wxListEvent& event)
{
    int iCol=event.GetColumn();
    // Sorting only allowed for the 4 first columns
    if ((iCol<BI_COL_NUMBER) || (iCol>BI_COL_VALUE)) return;
    m_bColOrderAsc[iCol]=!m_bColOrderAsc[iCol];
    m_DatasMngr.SortBills(iCol, m_bColOrderAsc[iCol]);
    RefreshBillsList();
}

void PanelNBookPage_Bills::OnUpdateUI_ItemButtons(wxUpdateUIEvent& event)
{
    event.Enable(m_lstBills->GetSelectedItemCount()!=0);
}

long PanelNBookPage_Bills::InsertBillItemToList(Bill* item, long before, bool select, bool updateWidths)
{
    if (item==NULL) return wxNOT_FOUND;

    long lBefore=(before==wxID_ANY?m_lstBills->GetItemCount():before);
    long lRes=m_lstBills->InsertItem(lBefore, wxEmptyString);

    m_lstBills->SetItem(lRes, BI_COL_NUMBER, wxString::Format(_T("%04d"), item->GetBillNumber()));
    m_lstBills->SetItem(lRes, BI_COL_DATE_CREATION, item->GetCreationDate().Format(_T("%d/%m/%Y")));
    m_lstBills->SetItem(lRes, BI_COL_CLIENT, m_DatasMngr.GetClientString(item->GetClientKey()));
    m_lstBills->SetItem(lRes, BI_COL_VALUE, wxString::Format(_T("%10.02f€"), item->GetTotalPrice()));
    m_lstBills->SetItem(lRes, BI_COL_STATUS, item->IsLocked()?_T("Payée"):_T("Non-payée"));
    m_lstBills->SetItem(lRes, BI_COL_DATE_LIMIT, item->GetTermDate().IsValid()?item->GetTermDate().Format(_T("%d/%m/%Y")):_T(""));

    m_lstBills->SetItemPtrData(lRes, wxUIntPtr(item));

    if (select)
    {
        m_lstBills->EnsureVisible(lRes);
        m_lstBills->Select(lRes);
    }
    if (updateWidths)
    {
        for (int i=0; i<m_lstBills->GetColumnCount(); ++i)
        {
            m_lstBills->SetColumnWidth(i, wxLIST_AUTOSIZE);
        }
    }
    return lRes;
}

void PanelNBookPage_Bills::RefreshBillsList()
{
    int iCount=m_DatasMngr.GetBillsCount();
    bool bLocked=m_optDisplay[1]->GetValue();
    m_lstBills->DeleteAllItems();
    for (int i=0; i<iCount; ++i)
    {
        Bill* item=m_DatasMngr.GetBill(i);
        if ((m_optDisplay[0]->GetValue()) || (item->IsLocked()==bLocked))
            InsertBillItemToList(item);
    }
    if (m_lstBills->GetItemCount())
    {
        for (int i=0; i<m_lstBills->GetColumnCount(); ++i)
        {
            m_lstBills->SetColumnWidth(i, wxLIST_AUTOSIZE);
        }
    }
}

void PanelNBookPage_Bills::UpdateBillItem(long lItem)
{
    if (lItem==wxNOT_FOUND) return;
    Bill* item=(Bill*)m_lstBills->GetItemData(lItem);
    if (item==NULL) return;
    m_lstBills->SetItem(lItem, BI_COL_NUMBER, wxString::Format(_T("%04d"), item->GetBillNumber()));
    m_lstBills->SetItem(lItem, BI_COL_DATE_CREATION, item->GetCreationDate().Format(_T("%d/%m/%Y")));
    m_lstBills->SetItem(lItem, BI_COL_CLIENT, m_DatasMngr.GetClientString(item->GetClientKey()));
    m_lstBills->SetItem(lItem, BI_COL_VALUE, wxString::Format(_T("%10.02f€"), item->GetTotalPrice()));
    m_lstBills->SetItem(lItem, BI_COL_STATUS, item->IsLocked()?_T("Payée"):_T("Non-payée"));
    m_lstBills->SetItem(lItem, BI_COL_DATE_LIMIT, item->GetTermDate().IsValid()?item->GetTermDate().Format(_T("%d/%m/%Y")): _T(""));

    for (int i=0; i<m_lstBills->GetColumnCount(); ++i)
    {
        m_lstBills->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
    }
}

void PanelNBookPage_Bills::OnBtnSaveAsClicked(wxCommandEvent& event)
{
    long lItem=m_lstBills->GetFirstSelected();
    if (lItem==wxNOT_FOUND) return;
    Bill* item=(Bill*)m_lstBills->GetItemData(lItem);
    if (item==NULL) return;
    wxString sFName=wxString::Format(_T("%04d Facture-"), item->GetBillNumber());
    sFName << item->GetCreationDate().Format(_T("%Y%m%d%H%M"));
    Client* cl=m_DatasMngr.GetClient(item->GetClientKey());
    if (cl==NULL) return;
    sFName << _T("-") << m_DatasMngr.GetClientString(cl) << _T(".pdf");

    wxFileDialog fdlg(wxTheApp->GetTopWindow(), _T("Enregistrer la facture"),
                      wxEmptyString, sFName, _T("Fichiers pdf|*.pdf|Tous les fichiers|*.*"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (fdlg.ShowModal()!=wxID_OK) return;

    wxPdfDocument *doc=m_DatasMngr.CreatePdfDoc(item);

    doc->SaveAsFile(fdlg.GetPath());
    delete doc;

    int iAction=m_SettingsMngr.GetPdfAction();

    if (iAction==PDA_NOTHING) return;

    if (iAction==PDA_ASK)
    {
        if (wxMessageBox(_T("Voulez-vous visualiser le fichier généré ?"), _T("Opération terminée"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES) return;
    }

    if (m_SettingsMngr.GetUseCustomPdfViewer())
    {
        wxString sCmd=_T("\"") + m_SettingsMngr.GetCustomPdfViewerPath() + _T("\"  \"") + fdlg.GetPath() + _T("\"");
        wxExecute(sCmd);
    }
    else
    {
        // Launch with default application
        wxLaunchDefaultApplication(fdlg.GetPath());
    }
}

void PanelNBookPage_Bills::OnBtnCopyClicked(wxCommandEvent& event)
{
    long lItem=m_lstBills->GetFirstSelected();
    if (lItem==wxNOT_FOUND) return;
    Bill* item=(Bill*)m_lstBills->GetItemData(lItem);
    if (item==NULL) return;

    Bill* newitem=m_DatasMngr.AddNewBill();
        newitem->SetCreationDate(item->GetCreationDate());
        newitem->SetTermDate(item->GetTermDate());
        newitem->SetClientKey(item->GetClientKey());
        newitem->SetLocked(item->IsLocked());
        int iCount=item->GetItemsCount(), iQty;
        wxString sDescr, sComm;
        double dPrice;
        for (int i=0; i<iCount; ++i)
        {
            sDescr=item->GetItemDescription(i);
            sComm=item->GetItemComments(i);
            iQty=item->GetItemQuantity(i);
            dPrice=item->GetItemUnitPrice(i);
            newitem->AddItem(sDescr, dPrice, sComm, iQty);
        }
        newitem->SetDiscount(item->GetDiscount());
        newitem->SetVAT(item->GetVAT());
    long lNew=InsertBillItemToList(newitem, wxID_ANY, true, true);

    DlgAddEditBill dlg(wxTheApp->GetTopWindow(), newitem);
    if (dlg.ShowModal()==wxID_OK)
    {
        UpdateBillItem(lNew);
    }
    else
    {
        m_DatasMngr.RemoveBill(newitem);
        m_lstBills->DeleteItem(lNew);
        m_lstBills->EnsureVisible(lItem);
        m_lstBills->Select(lItem);
    }
}

void PanelNBookPage_Bills::AddBillItem(Bill* item, bool select)
{
    InsertBillItemToList(item, wxID_ANY, true, true);
}

void PanelNBookPage_Bills::RefreshDatasLists()
{
    RefreshBillsList();
}
