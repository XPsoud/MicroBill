#include "dlgaddeditestimate.h"
#include <wx/statline.h>

#include "estimate.h"
#include "datasmanager.h"
#include "estimitemctrl.h"
#include "settingsmanager.h"
#include "dlgselectclient.h"
#include "dlgaddeditestimitem.h"

DlgAddEditEstimate::DlgAddEditEstimate(wxWindow *parent, Estimate* item) : wxDialog(parent, -1, _T("Add-Edit estimate")),
    m_options(SettingsManager::Get())
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgAddEditEstimate\" object\n"));
#endif // __WXDEBUG__
    m_sClient=wxEmptyString;
    m_bLocked=true;

    m_Estimate=item;
    if (item==NULL)
        SetTitle(_("Creating a new estimate"));
    else
        SetTitle(_("Editing an estimate"));

    CreateControls();

    ConnectControls();

    CenterOnParent();
}

DlgAddEditEstimate::~DlgAddEditEstimate()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgAddEditEstimate\" object\n"));
#endif // __WXDEBUG__
}

void DlgAddEditEstimate::CreateControls()
{
    wxBoxSizer *szrMain, *hszr, *vszr;
    wxStaticText *label;
    wxString sText;
    EstimItemCtrl eic(this);

    szrMain=new wxBoxSizer(wxVERTICAL);
        hszr=new wxBoxSizer(wxHORIZONTAL);
            label=new wxStaticText(this, wxID_STATIC, _("Creation date:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
            m_dpcCreation=new wxDatePickerCtrl(this, -1);
            hszr->Add(m_dpcCreation, 1, wxLEFT, 5);
            m_tpcCreation=new wxTimePickerCtrl(this, -1);
            hszr->Add(m_tpcCreation, 1, wxLEFT, 5);
            m_chkTermDate=new wxCheckBox(this, -1, _("Validity date:"));
            hszr->Add(m_chkTermDate, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
            m_dpcTerm=new wxDatePickerCtrl(this, -1);
            hszr->Add(m_dpcTerm, 1, wxLEFT, 5);
        szrMain->Add(hszr, 0, wxALL|wxEXPAND, 5);

        hszr=new wxBoxSizer(wxHORIZONTAL);
            label=new wxStaticText(this, wxID_STATIC, _("Reference client:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
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
        m_scwItems=new wxScrolledWindow(this, -1, wxDefaultPosition, wxSize(-1, 250), wxVSCROLL|wxHSCROLL|wxALWAYS_SHOW_SB);
#endif // __WXGTK__
            m_scwItems->SetBackgroundColour(*wxWHITE);
            wxBoxSizer *s=new wxBoxSizer(wxVERTICAL);
                m_pnlBottom=new wxPanel(m_scwItems);
                    vszr=new wxBoxSizer(wxVERTICAL);
                        sText = _("Total:");
                        sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%2.02f"));
                        m_lblTotal=new wxStaticText(m_pnlBottom, wxID_STATIC, sText, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_RIGHT);
                            m_lblTotal->SetFont(EstimItemCtrl::GetFixedFont());
                        vszr->Add(m_lblTotal, 0, wxALL|wxEXPAND, 5);
                        hszr=new wxBoxSizer(wxHORIZONTAL);
                            m_btnLock=new wxButton(m_pnlBottom, wxID_STOP, _("Lock/Unlock"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
                            hszr->Add(m_btnLock, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                            hszr->AddStretchSpacer(1);
                            label=new wxStaticText(m_pnlBottom, wxID_STATIC, _("Discount (%):"));
                            hszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                            m_scdDiscount=new wxSpinCtrlDouble(m_pnlBottom, -1, _T("0.00"), wxDefaultPosition, wxSize(60, -1));
                                m_scdDiscount->SetDigits(2);
                            hszr->Add(m_scdDiscount, 0, wxLEFT, 5);
                            sText = _("Amount:");
                            sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                            m_lblDiscount=new wxStaticText(m_pnlBottom, wxID_STATIC, sText);
                                m_lblDiscount->SetFont(EstimItemCtrl::GetFixedFont());
                            hszr->Add(m_lblDiscount, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        vszr->Add(hszr, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);
                        sText = _("Total w/o VAT:");
                        sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                        m_lblTotalDisc=new wxStaticText(m_pnlBottom, wxID_STATIC, sText, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_RIGHT);
                            m_lblTotalDisc->SetFont(EstimItemCtrl::GetFixedFont());
                        vszr->Add(m_lblTotalDisc, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
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
                                m_lblVAT->SetFont(EstimItemCtrl::GetFixedFont());
                            hszr->Add(m_lblVAT, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        vszr->Add(hszr, 0, wxALL|wxEXPAND, 5);
                        sText = _("Total to pay:");
                        sText << _T(" ") << m_options.GetFormatedMoneyValue(0., _T("%3.02f"));
                        m_lblTaxIncl=new wxStaticText(m_pnlBottom, wxID_STATIC, sText);
                            m_lblTaxIncl->SetFont(EstimItemCtrl::GetFixedFont());
                        vszr->Add(m_lblTaxIncl, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 5);
                    m_pnlBottom->SetSizer(vszr);
                s->Add(m_pnlBottom, 0, wxALL|wxEXPAND, 0);
            m_scwItems->SetSizer(s);
            m_scwItems->SetScrollRate(5, 5);
        szrMain->Add(m_scwItems, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
        s->Layout();

        szrMain->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxALL|wxEXPAND, 5);

    SetSizer(szrMain);

    if (m_Estimate!=NULL)
    {
        m_sClient=m_Estimate->GetClient();
        m_bLocked=!m_Estimate->IsLocked(); // Will be changed when updated

        Client *c=NULL;
        if (!m_sClient.IsEmpty())
            c=DatasManager::Get().GetClient(m_sClient);
        if (c!=NULL)
            m_txtClient->ChangeValue(DatasManager::Get().GetClientString(c));
        wxDateTime d=m_Estimate->GetCreationDate();
        m_dpcCreation->SetValue(d);
        m_tpcCreation->SetValue(d);
        d=m_Estimate->GetTermDate();
        if (d.IsValid())
        {
            m_chkTermDate->SetValue(true);
            m_dpcTerm->SetValue(d);
        }
        else
        {
            m_chkTermDate->SetValue(false);
        }
        int iCount=m_Estimate->GetItemsCount();
        for (int i=0; i<iCount; ++i)
        {
            wxString sDescr=m_Estimate->GetItemDescription(i);
            wxString sComms=m_Estimate->GetItemComments(i);
            double dPrice=m_Estimate->GetItemUnitPrice(i);
            int iQuantity=m_Estimate->GetItemQuantity(i);
            EstimItemCtrl* ectrl=new EstimItemCtrl(m_scwItems);
            ectrl->SetDatas(sDescr, sComms, dPrice, iQuantity);
            ectrl->SetItemIndex(i);
            s->Insert(i, ectrl, 0, wxALL|wxEXPAND, 0);
        }
        s->Layout();
        m_scdDiscount->SetValue(m_Estimate->GetDiscount());
        if (m_Estimate->GetVAT()!=0.)
        {
            m_chkShowVAT->SetValue(true);
            m_scdVAT->SetValue(m_Estimate->GetVAT());
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
    }
    wxCommandEvent evt(wxEVT_BUTTON, wxID_STOP);
    OnBtnLockClicked(evt);
    szrMain->SetSizeHints(this);
    Update();
}

void DlgAddEditEstimate::ConnectControls()
{
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnAddClicked, this, wxID_ADD);
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnDelClicked, this, wxID_DELETE);
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnEditClicked, this, wxID_EDIT);
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnMoveUpClicked, this, wxID_UP);
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnMoveDownClicked, this, wxID_DOWN);
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnLockClicked, this, wxID_STOP);
    m_btnSelClient->Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnSelClientClicked, this);
    Bind(wxEVT_BUTTON, &DlgAddEditEstimate::OnBtnValidClicked, this, wxID_OK);
    m_scdDiscount->Bind(wxEVT_SPINCTRLDOUBLE, &DlgAddEditEstimate::OnScdDiscountChanged, this);

    Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_BtnValid, this, wxID_OK);
    m_dpcTerm->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_DpcTerm, this);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_BtnMoveUp, this, wxID_UP);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_BtnMoveDown, this, wxID_DOWN);
    m_scdVAT->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_VATCtrls, this);
    m_lblVAT->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_VATCtrls, this);
    m_lblTaxIncl->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_VATCtrls, this);

    m_dpcCreation->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_tpcCreation->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_dpcTerm->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_chkTermDate->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_chkShowVAT->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_scdDiscount->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_btnSelClient->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    m_btnAdd->Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this, wxID_EDIT);
    Bind(wxEVT_UPDATE_UI, &DlgAddEditEstimate::OnUpdateUI_OthersCtrls, this, wxID_DELETE);
}

void DlgAddEditEstimate::OnUpdateUI_DpcTerm(wxUpdateUIEvent& event)
{
    event.Enable(m_chkTermDate->IsChecked());
}

void DlgAddEditEstimate::OnUpdateUI_BtnValid(wxUpdateUIEvent& event)
{
    bool bEnable=true;
    if (m_sClient.IsEmpty()) bEnable=false;

    event.Enable(bEnable);
}

void DlgAddEditEstimate::OnUpdateUI_BtnMoveUp(wxUpdateUIEvent& event)
{
    EstimItemCtrl* ctrl=(EstimItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    event.Enable((ctrl->GetItemIndex()!=0) && (!m_bLocked));
}

void DlgAddEditEstimate::OnUpdateUI_VATCtrls(wxUpdateUIEvent& event)
{
    wxControl* ctrl=(wxControl*)event.GetEventObject();
    if ((ctrl==m_lblTaxIncl) || (ctrl==m_lblVAT))
        event.Enable(m_chkShowVAT->IsChecked());
    else
        event.Enable(m_chkShowVAT->IsChecked() && (!m_bLocked));
}

void DlgAddEditEstimate::OnUpdateUI_BtnMoveDown(wxUpdateUIEvent& event)
{
    EstimItemCtrl* ctrl=(EstimItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    int iMax=m_scwItems->GetSizer()->GetItemCount()-2;
    event.Enable((ctrl->GetItemIndex()!=iMax) && (!m_bLocked));
}

void DlgAddEditEstimate::OnUpdateUI_OthersCtrls(wxUpdateUIEvent& event)
{
    event.Enable(!m_bLocked);
}

void DlgAddEditEstimate::OnBtnSelClientClicked(wxCommandEvent& event)
{
    DlgSelectClient dlg(this);
    if (dlg.ShowModal()!=wxID_OK) return;
    Client *c=dlg.GetClient();
    if (c!=NULL)
    {
        m_txtClient->ChangeValue(DatasManager::Get().GetClientString(c));
        m_sClient=c->GetKey();
    }
}

void DlgAddEditEstimate::OnBtnValidClicked(wxCommandEvent& event)
{
    if (m_Estimate!=NULL)
        SaveToEstimateItem(m_Estimate);
    EndModal(wxID_OK);
}

void DlgAddEditEstimate::SaveToEstimateItem(Estimate* item)
{
    if (item==NULL) return;
    item->SetClient(m_sClient);
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
        EstimItemCtrl *itmctrl=(EstimItemCtrl*)szr->GetItem(i)->GetWindow();
        item->AddItem(itmctrl->GetDescription(), itmctrl->GetUnitPrice(), itmctrl->GetComments(), itmctrl->GetQuantity());
    }
    item->SetLocked(m_bLocked);
    if (m_chkShowVAT->IsChecked())
        item->SetVAT(m_scdVAT->GetValue());
    else
        item->SetVAT(0.);
}

void DlgAddEditEstimate::OnBtnAddClicked(wxCommandEvent& event)
{
    DlgAddEditEstimItem dlg(this);
    if (dlg.ShowModal()!=wxID_OK) return;
    wxString sDescr=dlg.GetDescription();
    wxString sComms=dlg.GetComments();
    double dPrice=dlg.GetUnitPrice();
    int iQuantity=dlg.GetQuantity();
    EstimItemCtrl* ectrl=new EstimItemCtrl(m_scwItems);
    ectrl->SetDatas(sDescr, sComms, dPrice, iQuantity);
    wxSizer *szr=m_scwItems->GetSizer();
    int iLastIndex=szr->GetItemCount()-1;
    ectrl->SetItemIndex(iLastIndex);
    szr->Insert(iLastIndex, ectrl, 0, wxALL|wxEXPAND, 0);
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditEstimate::OnBtnDelClicked(wxCommandEvent& event)
{
    EstimItemCtrl* ctrl=(EstimItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    if (wxMessageBox(_("Remove this entry ?"), _("Confirmation"), wxICON_QUESTION|wxYES_NO|wxCENTER)!=wxYES)
        return;
    delete ctrl;
    wxSizer *szr=m_scwItems->GetSizer();
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditEstimate::OnBtnEditClicked(wxCommandEvent& event)
{
    EstimItemCtrl* ctrl=(EstimItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    DlgAddEditEstimItem dlg(this);
    dlg.SetItemDatas(ctrl->GetDescription(), ctrl->GetComments(), ctrl->GetUnitPrice(), ctrl->GetQuantity());
    if (dlg.ShowModal()!=wxID_OK)
        return;
    ctrl->SetDatas(dlg.GetDescription(), dlg.GetComments(), dlg.GetUnitPrice(), dlg.GetQuantity());
    GetSizer()->Layout();
    Update();
}

void DlgAddEditEstimate::OnBtnMoveUpClicked(wxCommandEvent& event)
{
    EstimItemCtrl* ctrl=(EstimItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
    int iItemIndex=ctrl->GetItemIndex();
    if (iItemIndex==0) return;
    wxSizer *szr=m_scwItems->GetSizer();
    szr->Detach(iItemIndex);
    szr->Insert(iItemIndex-1, ctrl, 0, wxALL|wxEXPAND, 0);
    szr->Layout();
    GetSizer()->Layout();
    Update();
}

void DlgAddEditEstimate::OnBtnMoveDownClicked(wxCommandEvent& event)
{
    EstimItemCtrl* ctrl=(EstimItemCtrl*)((wxWindow*)event.GetEventObject())->GetParent();
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

void DlgAddEditEstimate::OnScdDiscountChanged(wxSpinDoubleEvent& event)
{
    Update();
}

void DlgAddEditEstimate::OnBtnLockClicked(wxCommandEvent& event)
{
    m_bLocked=!m_bLocked;
    if (m_bLocked)
        m_btnLock->SetLabel(_("Mark as not finalized"));
    else
        m_btnLock->SetLabel(_("Mark as finalized"));
    m_btnLock->GetContainingSizer()->Layout();
}

void DlgAddEditEstimate::Update()
{
    wxSizer *szr=m_scwItems->GetSizer();
    double dTotal=0.;
    int iCount=szr->GetItemCount()-1;
    for (int i=0; i<iCount; ++i)
    {
        wxSizerItem *sitm=szr->GetItem(i);
        EstimItemCtrl* itm=(EstimItemCtrl*)sitm->GetWindow();
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
