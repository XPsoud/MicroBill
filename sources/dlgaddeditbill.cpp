#include "dlgaddeditbill.h"

#include <wx/statline.h>

#include "bill.h"
#include "datasmanager.h"
#include "billitemctrl.h"
#include "settingsmanager.h"
#include "dlgselectclient.h"
#include "dlgaddeditbillitem.h"

DlgAddEditBill::DlgAddEditBill(wxWindow *parent, Bill* item) : wxDialog(parent, -1, _T("Add-Edit client")),
    m_options(SettingsManager::Get())
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgAddEditBill\" object\n"));
#endif // __WXDEBUG__
    m_sClientKey=wxEmptyString;
    m_bLocked=true;

    m_Bill=item;
    if (item==NULL)
        SetTitle(_("Creating a new bill"));
    else
        SetTitle(_("Editing a bill"));

    CreateControls();

    ConnectControls();

    CheckClientDatas();

    CenterOnParent();
}

DlgAddEditBill::~DlgAddEditBill()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgAddEditBill\" object\n"));
#endif // __WXDEBUG__
}

void DlgAddEditBill::CreateControls()
{
    wxBoxSizer *szrMain, *hszr, *vszr;
    wxStaticText *label;
    wxString sText;
    BillItemCtrl bic(this);

    szrMain=new wxBoxSizer(wxVERTICAL);
        hszr=new wxBoxSizer(wxHORIZONTAL);
            label=new wxStaticText(this, wxID_STATIC, _("Creation date:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            m_dpcCreation=new wxDatePickerCtrl(this, -1);
            hszr->Add(m_dpcCreation, 1, wxLEFT, 5);
            m_tpcCreation=new wxTimePickerCtrl(this, -1);
            hszr->Add(m_tpcCreation, 1, wxLEFT, 5);
            m_chkTermDate=new wxCheckBox(this, -1, _("Payment deadline:"));
            hszr->Add(m_chkTermDate, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            m_dpcTerm=new wxDatePickerCtrl(this, -1);
            hszr->Add(m_dpcTerm, 1, wxLEFT, 5);
        szrMain->Add(hszr, 0, wxALL|wxEXPAND, 5);

        hszr=new wxBoxSizer(wxHORIZONTAL);
            label=new wxStaticText(this, wxID_STATIC, _("Nb:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            m_txtNumber=new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(45, -1), wxTE_READONLY);
            hszr->Add(m_txtNumber, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

            label=new wxStaticText(this, wxID_STATIC, _("Client:"));
            hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            m_txtClient=new wxTextCtrl(this, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
                wxFont fnt=m_txtClient->GetFont();
                fnt.SetWeight(wxFONTWEIGHT_BOLD);
                m_txtClient->SetFont(fnt);
            hszr->Add(m_txtClient, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
            m_btnSelClient=new wxButton(this, -1, _("Select"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
            hszr->Add(m_btnSelClient, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
        szrMain->Add(hszr, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
#ifdef __WXGTK__
        // Don't use the wxALWAYS_SHOW_SB style under Linux (gives an assertion message)
        m_scwItems=new wxScrolledWindow(this, -1, wxDefaultPosition, wxSize(-1, 250), wxVSCROLL|wxHSCROLL);
#else
        m_scwItems=new wxScrolledWindow(this, -1, wxDefaultPosition, wxSize(-1, 300), wxVSCROLL|wxHSCROLL|wxALWAYS_SHOW_SB);
#endif // __WXGTK__
            m_scwItems->SetBackgroundColour(*wxWHITE);
            wxBoxSizer *s=new wxBoxSizer(wxVERTICAL);
                m_pnlBottom=new wxPanel(m_scwItems);
                    vszr=new wxBoxSizer(wxVERTICAL);
                        sText = _("Total:");
                        sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%2.02f"));
                        m_lblTotal=new wxStaticText(m_pnlBottom, wxID_STATIC, sText, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_RIGHT);
                            m_lblTotal->SetFont(BillItemCtrl::GetFixedFont());
                        vszr->Add(m_lblTotal, 0, wxALL|wxEXPAND, 5);
                        hszr=new wxBoxSizer(wxHORIZONTAL);
                            m_btnLock=new wxButton(m_pnlBottom, wxID_STOP, _("Lock/Unlock"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                            hszr->Add(m_btnLock, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                            hszr->AddStretchSpacer(1);
                            label=new wxStaticText(m_pnlBottom, wxID_STATIC, _T("Discount (%):"));
                            hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                            m_scdDiscount=new wxSpinCtrlDouble(m_pnlBottom, -1, _T("0.00"), wxDefaultPosition, wxSize(60, -1));
                                m_scdDiscount->SetDigits(2);
                            hszr->Add(m_scdDiscount, 0, wxLEFT, 5);
                            sText = _("Amount:");
                            sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                            m_lblDiscount=new wxStaticText(m_pnlBottom, wxID_STATIC, sText);
                                m_lblDiscount->SetFont(BillItemCtrl::GetFixedFont());
                            hszr->Add(m_lblDiscount, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        vszr->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);
                        sText = _("Total w/o VAT:");
                        sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                        m_lblTotalDisc=new wxStaticText(m_pnlBottom, wxID_STATIC, sText, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_RIGHT);
                        vszr->Add(m_lblTotalDisc, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
                        hszr=new wxBoxSizer(wxHORIZONTAL);
                            m_chkPayNote=new wxCheckBox(m_pnlBottom, -1, _("Payment note:"));
                            hszr->Add(m_chkPayNote, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                            m_txtPayNote=new wxTextCtrl(m_pnlBottom, -1, wxEmptyString);
                            hszr->Add(m_txtPayNote, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        vszr->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);
                        hszr=new wxBoxSizer(wxHORIZONTAL);
                            m_btnAdd=new wxButton(m_pnlBottom, wxID_ADD, _("Add an entry"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                            hszr->Add(m_btnAdd, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                            hszr->AddStretchSpacer(1);
                            m_chkShowVAT=new wxCheckBox(m_pnlBottom, -1, _("Appy V.A.T.:"));
                            hszr->Add(m_chkShowVAT, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
// TODO (Xaviou#1#): Make VAT value customizable
                            m_scdVAT=new wxSpinCtrlDouble(m_pnlBottom, -1, _T("20.00"), wxDefaultPosition, wxSize(60, -1));
                                m_scdVAT->SetDigits(2);
                            hszr->Add(m_scdVAT, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                            sText = _("Amount:");
                            sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                            m_lblVAT=new wxStaticText(m_pnlBottom, wxID_STATIC, sText);
                                m_lblVAT->SetFont(BillItemCtrl::GetFixedFont());
                            hszr->Add(m_lblVAT, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        vszr->Add(hszr, 0, wxALL|wxEXPAND, 5);
                        sText = _("Total to pay:");
                        sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                        m_lblTaxIncl=new wxStaticText(m_pnlBottom, wxID_STATIC, sText);
                            m_lblTaxIncl->SetFont(BillItemCtrl::GetFixedFont());
                        vszr->Add(m_lblTaxIncl, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 5);
                    m_pnlBottom->SetSizer(vszr);
                s->Add(m_pnlBottom, 0, wxALL|wxEXPAND, 0);
            m_scwItems->SetSizer(s);
            m_scwItems->SetScrollRate(5, 5);
        szrMain->Add(m_scwItems, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
        s->Layout();

        szrMain->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxALL|wxEXPAND, 5);

    SetSizer(szrMain);

    if (m_Bill!=NULL)
    {
        SetBillNumber(m_Bill->GetBillNumber());
        m_sClientKey=m_Bill->GetClientKey();
        m_bLocked=!m_Bill->IsLocked(); // Will be changed when updated

        m_txtClient->ChangeValue(m_Bill->GetClientTitle());
        wxDateTime d=m_Bill->GetCreationDate();
        m_dpcCreation->SetValue(d);
        m_tpcCreation->SetValue(d);
        d=m_Bill->GetTermDate();
        if (d.IsValid())
        {
            m_chkTermDate->SetValue(true);
            m_dpcTerm->SetValue(d);
        }
        else
        {
            m_chkTermDate->SetValue(false);
        }
        int iCount=m_Bill->GetItemsCount();
        for (int i=0; i<iCount; ++i)
        {
            wxString sDescr=m_Bill->GetItemDescription(i);
            wxString sComms=m_Bill->GetItemComments(i);
            double dPrice=m_Bill->GetItemUnitPrice(i);
            int iQuantity=m_Bill->GetItemQuantity(i);
            BillItemCtrl* ectrl=new BillItemCtrl(m_scwItems);
            ectrl->SetDatas(sDescr, sComms, dPrice, iQuantity);
            ectrl->SetItemIndex(i);
            s->Insert(i, ectrl, 0, wxALL|wxEXPAND, 0);
        }
        s->Layout();
        m_txtPayNote->ChangeValue(m_Bill->GetPayNote());
        bool bPayNote=(m_txtPayNote->IsEmpty()==false);
        m_chkPayNote->SetValue(bPayNote);
        m_txtPayNote->Enable(bPayNote);
        m_scdDiscount->SetValue(m_Bill->GetDiscount());
        if (m_Bill->GetVAT()!=0.)
        {
            m_chkShowVAT->SetValue(true);
            m_scdVAT->SetValue(m_Bill->GetVAT());
        }
        else
        {
            m_chkShowVAT->SetValue(false);
        }
    }
    else
    {
        wxDateTime dt=wxDateTime::Now();
        m_dpcCreation->SetValue(dt);
        m_tpcCreation->SetValue(dt);
        dt += wxDateSpan(0, 0, 0, 15);
        m_dpcTerm->SetValue(dt);
        m_chkPayNote->SetValue(false);
        m_txtPayNote->ChangeValue(wxEmptyString);
        m_txtPayNote->Disable();
    }
    wxCommandEvent evt(wxEVT_BUTTON, wxID_STOP);
    OnBtnLockClicked(evt);
    szrMain->SetSizeHints(this);
    Update();
}

void DlgAddEditBill::ConnectControls()
{
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnAddClicked, this, wxID_ADD);
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnDelClicked, this, wxID_DELETE);
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnEditClicked, this, wxID_EDIT);
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnMoveUpClicked, this, wxID_UP);
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnMoveDownClicked, this, wxID_DOWN);
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnLockClicked, this, wxID_STOP);
    m_btnSelClient->Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnSelClientClicked, this);
    Bind(wxEVT_BUTTON, &DlgAddEditBill::OnBtnValidClicked, this, wxID_OK);
    m_scdDiscount->Bind(wxEVT_SPINCTRLDOUBLE, &DlgAddEditBill::OnScdDiscountChanged, this);
    m_chkPayNote->Bind(wxEVT_CHECKBOX, &DlgAddEditBill::OnChkPayNoteClicked, this);

    Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_BtnValid, this, wxID_OK);
    m_dpcTerm->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_DpcTerm, this);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_BtnMoveUp, this, wxID_UP);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_BtnMoveDown, this, wxID_DOWN);
    m_scdVAT->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_VATCtrls, this);
    m_lblVAT->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_VATCtrls, this);
    m_lblTaxIncl->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_VATCtrls, this);

    m_dpcCreation->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_tpcCreation->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_dpcTerm->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_chkTermDate->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_chkShowVAT->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_scdDiscount->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_btnSelClient->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    m_btnAdd->Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this, wxID_EDIT);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditBill::OnUpdateUI_OthersCtrls, this, wxID_DELETE);
}

void DlgAddEditBill::SetBillNumber(int number)
{
    m_txtNumber->ChangeValue(wxString::Format(_T("%04d"), number));
}

void DlgAddEditBill::OnUpdateUI_DpcTerm(wxUpdateUIEvent& event)
{
    event.Enable(m_chkTermDate->IsChecked());
}

void DlgAddEditBill::OnUpdateUI_BtnValid(wxUpdateUIEvent& event)
{
    bool bEnable=true;
    if (m_sClientKey.IsEmpty()) bEnable=false;

    event.Enable(bEnable);
}

void DlgAddEditBill::OnUpdateUI_BtnMoveUp(wxUpdateUIEvent& event)
{
    BillItemCtrl* ctrl=(BillItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    event.Enable(ctrl->GetItemIndex()!=0);
}

void DlgAddEditBill::OnUpdateUI_VATCtrls(wxUpdateUIEvent& event)
{
    wxControl* ctrl=(wxControl*)event.GetEventObject();
    if ((ctrl==m_lblTaxIncl) || (ctrl==m_lblVAT))
        event.Enable(m_chkShowVAT->IsChecked());
    else
        event.Enable(m_chkShowVAT->IsChecked() && (!m_bLocked));
}

void DlgAddEditBill::OnUpdateUI_BtnMoveDown(wxUpdateUIEvent& event)
{
    BillItemCtrl* ctrl=(BillItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    int iMax=m_scwItems->GetSizer()->GetItemCount()-2;
    event.Enable(ctrl->GetItemIndex()!=iMax);
}

void DlgAddEditBill::OnUpdateUI_OthersCtrls(wxUpdateUIEvent& event)
{
    event.Enable(!m_bLocked);
}

void DlgAddEditBill::OnBtnSelClientClicked(wxCommandEvent& event)
{
    DlgSelectClient dlg(this);
    if (dlg.ShowModal()!=wxID_OK) return;
    Client *c=dlg.GetClient();
    if (c!=NULL)
    {
        m_sClientKey=c->GetKey();
        m_txtClient->ChangeValue(DatasManager::Get().GetClientString(c));
    }
}

void DlgAddEditBill::OnBtnValidClicked(wxCommandEvent& event)
{
    if (m_Bill!=NULL)
        SaveToBillItem(m_Bill);
    EndModal(wxID_OK);
}

void DlgAddEditBill::SaveToBillItem(Bill* item)
{
    if (item==NULL) return;
    item->SetClientKey(m_sClientKey);
    item->UpdateClientDatas();
    wxDateTime d;
    d=m_dpcCreation->GetValue();
    wxString s=d.Format(_T("%Y-%m-%d"));
    d=m_tpcCreation->GetValue();
    s << _T(" ") << d.Format(_T("%H:%M:%S"));
    wxString::const_iterator sit;
    d.ParseDateTime(s, &sit);
    item->SetCreationDate(d);
    if (m_chkTermDate->IsChecked())
    {
        item->SetTermDate(m_dpcTerm->GetValue());
    }
    else
    {
        item->SetTermDate(wxInvalidDateTime);
    }
    item->SetDiscount(m_scdDiscount->GetValue());
    item->RemoveAllItems();
    wxSizer *szr=m_scwItems->GetSizer();
    int iCount=szr->GetItemCount()-1;
    for (int i=0; i<iCount; ++i)
    {
        BillItemCtrl *itmctrl=(BillItemCtrl*)szr->GetItem(i)->GetWindow();
        item->AddItem(itmctrl->GetDescription(), itmctrl->GetUnitPrice(), itmctrl->GetComments(), itmctrl->GetQuantity());
    }
    item->SetLocked(m_bLocked);
    if (m_chkShowVAT->IsChecked())
        item->SetVAT(m_scdVAT->GetValue());
    else
        item->SetVAT(0.);

    if (m_chkPayNote->IsChecked())
        item->SetPayNote(m_txtPayNote->GetValue());
    else
        item->SetPayNote(wxEmptyString);
}

void DlgAddEditBill::OnBtnAddClicked(wxCommandEvent& event)
{
    DlgAddEditBillItem dlg(this);
    if (dlg.ShowModal()!=wxID_OK) return;
    wxString sDescr=dlg.GetDescription();
    wxString sComms=dlg.GetComments();
    double dPrice=dlg.GetUnitPrice();
    int iQuantity=dlg.GetQuantity();
    BillItemCtrl* ectrl=new BillItemCtrl(m_scwItems);
    ectrl->SetDatas(sDescr, sComms, dPrice, iQuantity);
    wxSizer *szr=m_scwItems->GetSizer();
    int iLastIndex=szr->GetItemCount()-1;
    ectrl->SetItemIndex(iLastIndex);
    szr->Insert(iLastIndex, ectrl, 0, wxALL|wxEXPAND, 0);
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditBill::OnBtnDelClicked(wxCommandEvent& event)
{
    BillItemCtrl* ctrl=(BillItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    if (wxMessageBox(_("Remove this entry ?"), _("Confirmation"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES)
        return;
    delete ctrl;
    wxSizer *szr=m_scwItems->GetSizer();
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditBill::OnBtnEditClicked(wxCommandEvent& event)
{
    BillItemCtrl* ctrl=(BillItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    DlgAddEditBillItem dlg(this);
    dlg.SetItemDatas(ctrl->GetDescription(), ctrl->GetComments(), ctrl->GetUnitPrice(), ctrl->GetQuantity());
    if (dlg.ShowModal()!=wxID_OK)
        return;
    ctrl->SetDatas(dlg.GetDescription(), dlg.GetComments(), dlg.GetUnitPrice(), dlg.GetQuantity());
    GetSizer()->Layout();
    Update();
}

void DlgAddEditBill::OnBtnMoveUpClicked(wxCommandEvent& event)
{
    BillItemCtrl* ctrl=(BillItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    int iItemIndex=ctrl->GetItemIndex();
    if (iItemIndex==0) return;
    wxSizer *szr=m_scwItems->GetSizer();
    szr->Detach(iItemIndex);
    szr->Insert(iItemIndex-1, ctrl, 0, wxALL|wxEXPAND, 0);
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditBill::OnBtnMoveDownClicked(wxCommandEvent& event)
{
    BillItemCtrl* ctrl=(BillItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    wxSizer *szr=m_scwItems->GetSizer();
    int iItemIndex=ctrl->GetItemIndex();
    int iMax=szr->GetItemCount()-2;
    if (iItemIndex>=iMax) return;
    szr->Detach(iItemIndex);
    szr->Insert(iItemIndex+1, ctrl, 0, wxALL|wxEXPAND, 0);
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditBill::OnScdDiscountChanged(wxSpinDoubleEvent& event)
{
    Update();
}

void DlgAddEditBill::OnChkPayNoteClicked(wxCommandEvent& event)
{
    m_txtPayNote->Enable(m_chkPayNote->IsChecked());
}

void DlgAddEditBill::OnBtnLockClicked(wxCommandEvent& event)
{
    m_bLocked=!m_bLocked;
    if (m_bLocked)
        m_btnLock->SetLabel(_("Mark as non paid"));
    else
        m_btnLock->SetLabel(_("Mark as paid"));
    m_btnLock->GetContainingSizer()->Layout();
    CheckClientDatas();
}

void DlgAddEditBill::Update()
{
    wxSizer *szr=m_scwItems->GetSizer();
    double dTotal=0.;
    int iCount=szr->GetItemCount()-1;
    for (int i=0; i<iCount; ++i)
    {
        wxSizerItem *sitm=szr->GetItem(i);
        BillItemCtrl* itm=(BillItemCtrl*)sitm->GetWindow();
        itm->SetItemIndex(i);
        dTotal += itm->GetQuantity() * itm->GetUnitPrice();
    }
    wxString sText = _("Total:");
    sText << _T(" ") << m_options.GetFormatedMoneyValue(dTotal, _T("%8.02f"));
    m_lblTotal->SetLabel(sText);
    double dDiscount=(m_scdDiscount->GetValue()/100.)*dTotal;
    sText = _("Amount:");
    sText << _T(" ") << m_options.GetFormatedMoneyValue(dDiscount, _T("%8.02f"));
    m_lblDiscount->SetLabel(sText);
    dTotal -= dDiscount;
    sText = _("Total w/o VAT:");
    sText << _T(" ") << m_options.GetFormatedMoneyValue(dTotal, _T("%8.02f"));
    m_lblTotalDisc->SetLabel(sText);
    double dVat=(m_scdVAT->GetValue()/100.)*dTotal;
    sText = _("Amount:");
    sText << _T(" ") << m_options.GetFormatedMoneyValue(dVat, _T("%8.02f"));
    m_lblVAT->SetLabel(sText);
    dTotal += dVat;
    sText = _("Total to pay:");
    sText << _T(" ") << m_options.GetFormatedMoneyValue(dTotal, _T("%8.02f"));
    m_lblTaxIncl->SetLabel(sText);
    m_pnlBottom->GetSizer()->Layout();
}

void DlgAddEditBill::CheckClientDatas()
{
    if ((m_sClientKey.IsEmpty()) || (m_Bill==NULL)) return;
    if (!m_Bill->IsClientUpToDate() && !m_bLocked)
    {
        m_txtClient->SetForegroundColour(_T("orange"));
        m_txtClient->SetToolTip(_("The client datas for this bill are obsoletes.\nThey will be updated during the validation of this form."));
    }
    else
    {
        m_txtClient->SetForegroundColour(*wxBLACK);
        m_txtClient->SetToolTip(wxEmptyString);
    }
    m_txtClient->Refresh();
}
