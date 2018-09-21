#include "panelnbookpage_bills.h"
// TODO (Xaviou#1#): See "Clients panel" to manage columns widths
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
        hszr->Add(new wxStaticText(this, wxID_STATIC, _("View:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(5);
        m_optDisplay[0]=new wxRadioButton(this, -1, _("All bills"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
            m_optDisplay[0]->SetValue(true);
        hszr->Add(m_optDisplay[0], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        m_optDisplay[1]=new wxRadioButton(this, -1, _("Paid bills"));
        hszr->Add(m_optDisplay[1], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        m_optDisplay[2]=new wxRadioButton(this, -1, _("Unpaid bills"));
        hszr->Add(m_optDisplay[2], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    GetMainSizer()->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    hszr=new wxBoxSizer(wxHORIZONTAL);
        m_lstBills=new wxListView(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES);
        hszr->Add(m_lstBills, 1, wxALL|wxEXPAND, 0);
            m_lstBills->AppendColumn(_("Num"), wxLIST_FORMAT_RIGHT);
            m_lstBills->AppendColumn(_("Date"));
            m_lstBills->AppendColumn(_("Client"));
            m_lstBills->AppendColumn(_("Amount"), wxLIST_FORMAT_RIGHT);
            m_lstBills->AppendColumn(_("Status"), wxLIST_FORMAT_CENTER);
            m_lstBills->AppendColumn(_("Deadline date"));
        hszr->AddSpacer(5);
        vszr=new wxBoxSizer(wxVERTICAL);
            m_btnAdd=new wxButton(this, wxID_ADD, wxGetStockLabel(wxID_ADD, wxSTOCK_FOR_BUTTON));
            vszr->Add(m_btnAdd, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnDel=new wxButton(this, wxID_DELETE, wxGetStockLabel(wxID_DELETE, wxSTOCK_FOR_BUTTON));
                m_btnDel->Disable();
            vszr->Add(m_btnDel, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnEdit=new wxButton(this, wxID_EDIT, _("Modify"));
                m_btnEdit->Disable();
            vszr->Add(m_btnEdit, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(15);
            m_btnSaveAs=new wxButton(this, wxID_SAVEAS, _("Export"));
                m_btnSaveAs->Disable();
            vszr->Add(m_btnSaveAs, 0, wxALL|wxEXPAND, 0);
            vszr->AddSpacer(5);
            m_btnCopy=new wxButton(this, wxID_COPY, _("Duplicate"));
                m_btnCopy->Disable();
            vszr->Add(m_btnCopy, 0, wxALL|wxEXPAND, 0);
        hszr->Add(vszr, 0, wxALL|wxEXPAND, 0);
    GetMainSizer()->Add(hszr, 1, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    GetMainSizer()->AddSpacer(5);
}

void PanelNBookPage_Bills::ConnectControls()
{
    for (int i=0; i<3; ++i)
        m_optDisplay[i]->Bind(wxEVT_RADIOBUTTON, &PanelNBookPage_Bills::OnOptDisplayClicked, this);

    m_btnAdd->Bind(wxEVT_BUTTON, &PanelNBookPage_Bills::OnBtnAddClicked, this);
    m_btnDel->Bind(wxEVT_BUTTON, &PanelNBookPage_Bills::OnBtnDelClicked, this);
    m_btnEdit->Bind(wxEVT_BUTTON, &PanelNBookPage_Bills::OnBtnEditClicked, this);
    m_btnSaveAs->Bind(wxEVT_BUTTON, &PanelNBookPage_Bills::OnBtnSaveAsClicked, this);
    m_btnCopy->Bind(wxEVT_BUTTON, &PanelNBookPage_Bills::OnBtnCopyClicked, this);
    m_lstBills->Bind(wxEVT_LIST_ITEM_ACTIVATED, &PanelNBookPage_Bills::OnLstItemActivated, this);
    m_lstBills->Bind(wxEVT_LIST_COL_CLICK, &PanelNBookPage_Bills::OnLstBillsColumnClicked, this);

    m_btnDel->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Bills::OnUpdateUI_ItemButtons, this);
    m_btnEdit->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Bills::OnUpdateUI_ItemButtons, this);
    m_btnSaveAs->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Bills::OnUpdateUI_ItemButtons, this);
    m_btnCopy->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Bills::OnUpdateUI_ItemButtons, this);
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
    if (wxMessageBox(_("Remove the selected bill?"), _("Confirmation"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES)
        return;
    if (!m_DatasMngr.RemoveBill(item))
    {
        wxMessageBox(_("An error occurred while removing the selected bill!"), _("Error"), wxICON_ERROR|wxCENTER|wxOK);
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
    m_lstBills->SetItem(lRes, BI_COL_DATE_CREATION, item->GetCreationDate().Format(_("%Y-%m-%d")));
    m_lstBills->SetItem(lRes, BI_COL_CLIENT, m_DatasMngr.GetClientString(item->GetClientKey()));
    m_lstBills->SetItem(lRes, BI_COL_VALUE, wxString::Format(_("%10.02f$"), item->GetTotalPrice()));
    m_lstBills->SetItem(lRes, BI_COL_STATUS, item->IsLocked()?_("Paid"):_("Not paid"));
    m_lstBills->SetItem(lRes, BI_COL_DATE_LIMIT, item->GetTermDate().IsValid()?item->GetTermDate().Format(_("%Y-%m-%d")):_T(""));

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
    m_lstBills->SetItem(lItem, BI_COL_DATE_CREATION, item->GetCreationDate().Format(_("%Y-%m-%d")));
    m_lstBills->SetItem(lItem, BI_COL_CLIENT, m_DatasMngr.GetClientString(item->GetClientKey()));
    m_lstBills->SetItem(lItem, BI_COL_VALUE, wxString::Format(_("%10.02f$"), item->GetTotalPrice()));
    m_lstBills->SetItem(lItem, BI_COL_STATUS, item->IsLocked()?_("Paid"):_("Not paid"));
    m_lstBills->SetItem(lItem, BI_COL_DATE_LIMIT, item->GetTermDate().IsValid()?item->GetTermDate().Format(_("%Y-%m-%d")): _T(""));

    for (int i=0; i<m_lstBills->GetColumnCount(); ++i)
    {
        m_lstBills->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
    }
}

void PanelNBookPage_Bills::OnBtnSaveAsClicked(wxCommandEvent& event)
{
    // TODO (Xaviou#1#): Implement this using wxPdfDocument
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
