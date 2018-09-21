#include "panelnbookpage_estimates.h"

#include "main.h"
#include "dlgaddeditestimate.h"

#include <wx/mimetype.h>

const wxEventType wxEVT_CONVERT_ESTIM2BILL = wxNewEventType();

PanelNBookPage_Estimates::PanelNBookPage_Estimates(wxWindow* parent) :
    PanelNBookPage(parent, PNBP_TYPE_ESTIMATES)
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"PanelNBookPage_Estimates\" object\n"));
#endif // __WXDEBUG__
    for (int i=0; i<3; ++i) m_bColOrderAsc[i]=false;
    CreateControls();
    RefreshEstimatesList();
    ConnectControls();
}

PanelNBookPage_Estimates::~PanelNBookPage_Estimates()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"PanelNBookPage_Estimates\" object\n"));
#endif // __WXDEBUG__
}

void PanelNBookPage_Estimates::CreateControls()
{
    wxBoxSizer *hszr, *vszr;

    hszr=new wxBoxSizer(wxHORIZONTAL);
        hszr->Add(new wxStaticText(this, wxID_STATIC, _("View:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(5);
        m_optDisplay[0]=new wxRadioButton(this, -1, _("All estimates"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
            m_optDisplay[0]->SetValue(true);
        hszr->Add(m_optDisplay[0], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        m_optDisplay[1]=new wxRadioButton(this, -1, _("Finalized estimates"));
        hszr->Add(m_optDisplay[1], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
        hszr->AddSpacer(10);
        m_optDisplay[2]=new wxRadioButton(this, -1, _("Non-finalized estimates"));
        hszr->Add(m_optDisplay[2], 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    GetMainSizer()->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    hszr=new wxBoxSizer(wxHORIZONTAL);
        m_lstEstimates=new wxListView(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES);
        hszr->Add(m_lstEstimates, 1, wxALL|wxEXPAND, 0);
            m_lstEstimates->AppendColumn(_("Date"));
            m_lstEstimates->AppendColumn(_("Client"));
            m_lstEstimates->AppendColumn(_("Amount"), wxLIST_FORMAT_RIGHT);
            m_lstEstimates->AppendColumn(_("Status"), wxLIST_FORMAT_CENTER);
            m_lstEstimates->AppendColumn(_("Validity"));
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
            vszr->AddSpacer(5);
            m_btnBill=new wxButton(this, wxID_CONVERT, _("Invoice"));
                m_btnBill->Disable();
            vszr->Add(m_btnBill, 0, wxALL|wxEXPAND, 0);
        hszr->Add(vszr, 0, wxALL|wxEXPAND, 0);
    GetMainSizer()->Add(hszr, 1, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

    GetMainSizer()->AddSpacer(5);

    // Get minimal columns widths
    for (int i=0; i<EI_COLS_COUNT; ++i)
    {
        m_lstEstimates->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
        m_iMinWidth[i] = m_lstEstimates->GetColumnWidth(i);
    }
}

void PanelNBookPage_Estimates::ConnectControls()
{
    for (int i=0; i<3; ++i)
        m_optDisplay[i]->Bind(wxEVT_RADIOBUTTON, &PanelNBookPage_Estimates::OnOptDisplayClicked, this);

    m_btnAdd->Bind(wxEVT_BUTTON, &PanelNBookPage_Estimates::OnBtnAddClicked, this);
    m_btnDel->Bind(wxEVT_BUTTON, &PanelNBookPage_Estimates::OnBtnDelClicked, this);
    m_btnEdit->Bind(wxEVT_BUTTON, &PanelNBookPage_Estimates::OnBtnEditClicked, this);
    m_btnSaveAs->Bind(wxEVT_BUTTON, &PanelNBookPage_Estimates::OnBtnSaveAsClicked, this);
    m_btnCopy->Bind(wxEVT_BUTTON, &PanelNBookPage_Estimates::OnBtnCopyClicked, this);
    m_btnBill->Bind(wxEVT_BUTTON, &PanelNBookPage_Estimates::OnBtnBillClicked, this);
    m_lstEstimates->Bind(wxEVT_LIST_ITEM_ACTIVATED, &PanelNBookPage_Estimates::OnLstItemActivated, this);
    m_lstEstimates->Bind(wxEVT_LIST_COL_CLICK, &PanelNBookPage_Estimates::OnLstEstimatesColumnClicked, this);

    m_btnDel->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Estimates::OnUpdateUI_ItemButtons, this);
    m_btnEdit->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Estimates::OnUpdateUI_ItemButtons, this);
    m_btnSaveAs->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Estimates::OnUpdateUI_ItemButtons, this);
    m_btnCopy->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Estimates::OnUpdateUI_ItemButtons, this);
    m_btnBill->Bind(wxEVT_UPDATE_UI, &PanelNBookPage_Estimates::OnUpdateUI_ItemButtons, this);
}

void PanelNBookPage_Estimates::OnOptDisplayClicked(wxCommandEvent& event)
{
    RefreshEstimatesList();
}

void PanelNBookPage_Estimates::OnBtnAddClicked(wxCommandEvent& event)
{
    DlgAddEditEstimate dlg(wxTheApp->GetTopWindow());
    if (dlg.ShowModal()==wxID_OK)
    {
        Estimate* item=m_DatasMngr.AddNewEstimate();
        dlg.SaveToEstimateItem(item);
        InsertEstimateItemToList(item, wxID_ANY, true, true);
    }
}

void PanelNBookPage_Estimates::OnBtnDelClicked(wxCommandEvent& event)
{
    long lRes=m_lstEstimates->GetFirstSelected();
    if (lRes==wxNOT_FOUND) return;
    Estimate* item=(Estimate*)m_lstEstimates->GetItemData(lRes);
    if (wxMessageBox(_("Remove the selected estimate?"), _("Confirmation"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES)
        return;
    if (!m_DatasMngr.RemoveEstimate(item))
    {
        wxMessageBox(_("An error occurred while removing the selected estimate!"), _("Error"), wxICON_ERROR|wxCENTER|wxOK);
        return;
    }
    m_lstEstimates->DeleteItem(lRes);
}

void PanelNBookPage_Estimates::OnBtnEditClicked(wxCommandEvent& event)
{
    long lRes=m_lstEstimates->GetFirstSelected();
    if (lRes==wxNOT_FOUND) return;
    Estimate* item=(Estimate*)m_lstEstimates->GetItemData(lRes);
    DlgAddEditEstimate dlg(wxTheApp->GetTopWindow(), item);
    if (dlg.ShowModal()==wxID_OK)
        UpdateEstimateItem(lRes);
}

void PanelNBookPage_Estimates::OnLstItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, wxID_EDIT);
    OnBtnEditClicked(evt);
}

void PanelNBookPage_Estimates::OnLstEstimatesColumnClicked(wxListEvent& event)
{
    int iCol=event.GetColumn();

    if ((iCol<0) || (iCol>5)) return;
    m_bColOrderAsc[iCol]=!m_bColOrderAsc[iCol];
    m_DatasMngr.SortEstimates(iCol, m_bColOrderAsc[iCol]);
    RefreshEstimatesList();
}

void PanelNBookPage_Estimates::OnUpdateUI_ItemButtons(wxUpdateUIEvent& event)
{
    event.Enable(m_lstEstimates->GetSelectedItemCount()!=0);
}

long PanelNBookPage_Estimates::InsertEstimateItemToList(Estimate* item, long before, bool select, bool updateWidths)
{
    if (item==NULL) return wxNOT_FOUND;

    long lBefore=(before==wxID_ANY?m_lstEstimates->GetItemCount():before);
    long lRes=m_lstEstimates->InsertItem(lBefore, wxEmptyString);

    wxString sItem;

    for (int i=0; i<EI_COLS_COUNT; ++i)
    {
        switch(i)
        {
            case EI_COL_DATE_CREATION:
                sItem = item->GetCreationDate().Format(_("%Y-%m-%d"));
                break;
            case EI_COL_CLIENT:
                sItem = m_DatasMngr.GetClientString(item->GetClient());
                break;
            case EI_COL_VALUE:
                sItem = wxString::Format(_("%10.02f$"), item->GetTotalPrice());
                break;
            case EI_COL_STATUS:
                sItem = item->IsLocked()?_("Finalized"):_("In progress");
                break;
            case EI_COL_DATE_LIMIT:
                sItem = item->GetTermDate().IsValid()?item->GetTermDate().Format(_("%Y-%m-%d")):_T("");
                break;
        }
        m_lstEstimates->SetItem(lRes, i, sItem);

        if ((!sItem.IsEmpty()) && updateWidths)
        {
            m_lstEstimates->SetColumnWidth(i, wxLIST_AUTOSIZE);
            int iWdth = m_lstEstimates->GetColumnWidth(i);
            if (iWdth < m_iMinWidth[i])
                m_lstEstimates->SetColumnWidth(i, m_iMinWidth[i]);
        }
    }

    m_lstEstimates->SetItemPtrData(lRes, wxUIntPtr(item));

    if (select)
    {
        m_lstEstimates->EnsureVisible(lRes);
        m_lstEstimates->Select(lRes);
    }

    return lRes;
}

void PanelNBookPage_Estimates::RefreshEstimatesList()
{
    int iCount=m_DatasMngr.GetEstimatesCount();
    bool bLocked=m_optDisplay[1]->GetValue();
    m_lstEstimates->DeleteAllItems();
    for (int i=0; i<iCount; ++i)
    {
        Estimate* item=m_DatasMngr.GetEstimate(i);
        if ((m_optDisplay[0]->GetValue()) || (item->IsLocked()==bLocked))
            InsertEstimateItemToList(item);
    }
    if (m_lstEstimates->GetItemCount())
    {
        for (int i=0; i<m_lstEstimates->GetColumnCount(); ++i)
        {
            m_lstEstimates->SetColumnWidth(i, wxLIST_AUTOSIZE);
            int iWdth = m_lstEstimates->GetColumnWidth(i);
            if (iWdth < m_iMinWidth[i])
                m_lstEstimates->SetColumnWidth(i, m_iMinWidth[i]);
        }
    }
}

void PanelNBookPage_Estimates::UpdateEstimateItem(long lItem)
{
    if (lItem==wxNOT_FOUND) return;
    Estimate* item=(Estimate*)m_lstEstimates->GetItemData(lItem);
    if (item==NULL) return;
    m_lstEstimates->SetItem(lItem, EI_COL_DATE_CREATION, item->GetCreationDate().Format(_("%Y-%m-%d")));
    m_lstEstimates->SetItem(lItem, EI_COL_CLIENT, m_DatasMngr.GetClientString(item->GetClient()));
    m_lstEstimates->SetItem(lItem, EI_COL_VALUE, wxString::Format(_("%10.02f$"), item->GetTotalPrice()));
    m_lstEstimates->SetItem(lItem, EI_COL_STATUS, item->IsLocked()?_("Finalized"):_("In progress"));
    m_lstEstimates->SetItem(lItem, EI_COL_DATE_LIMIT, item->GetTermDate().IsValid()?item->GetTermDate().Format(_("%Y-%m-%d")):_T(""));

    for (int i=0; i<m_lstEstimates->GetColumnCount(); ++i)
    {
        m_lstEstimates->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
    }
}

void PanelNBookPage_Estimates::OnBtnSaveAsClicked(wxCommandEvent& event)
{
    // TODO (Xaviou#1#): Implement this using wxPdfDocument
}

void PanelNBookPage_Estimates::OnBtnCopyClicked(wxCommandEvent& event)
{
    long lItem=m_lstEstimates->GetFirstSelected();
    if (lItem==wxNOT_FOUND) return;
    Estimate* item=(Estimate*)m_lstEstimates->GetItemData(lItem);
    if (item==NULL) return;

    Estimate* newitem=m_DatasMngr.AddNewEstimate();
        newitem->SetCreationDate(item->GetCreationDate());
        newitem->SetTermDate(item->GetTermDate());
        newitem->SetClient(item->GetClient());
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
    long lNew=InsertEstimateItemToList(newitem, wxID_ANY, true, true);

    DlgAddEditEstimate dlg(wxTheApp->GetTopWindow(), newitem);
    if (dlg.ShowModal()==wxID_OK)
    {
        UpdateEstimateItem(lNew);
    }
    else
    {
        m_DatasMngr.RemoveEstimate(newitem);
        m_lstEstimates->DeleteItem(lNew);
        m_lstEstimates->EnsureVisible(lItem);
        m_lstEstimates->Select(lItem);
    }
}

void PanelNBookPage_Estimates::OnBtnBillClicked(wxCommandEvent& event)
{
    long lItem=m_lstEstimates->GetFirstSelected();
    if (lItem==wxNOT_FOUND) return;
    Estimate* item=(Estimate*)m_lstEstimates->GetItemData(lItem);
    if (item==NULL) return;

    wxCommandEvent evt(wxEVT_CONVERT_ESTIM2BILL, wxID_ANY);
    evt.SetEventObject((wxObject*)item);
    AddPendingEvent(evt);
}

void PanelNBookPage_Estimates::RefreshDatasLists()
{
    RefreshEstimatesList();
}
