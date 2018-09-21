#ifndef __DLGADDEDITBILL_H_INCLUDED__
#define __DLGADDEDITBILL_H_INCLUDED__

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/spinctrl.h>

class Bill;

class DlgAddEditBill : public wxDialog
{
    public:
        DlgAddEditBill(wxWindow *parent, Bill* item=NULL);
        virtual ~DlgAddEditBill();
        void SaveToBillItem(Bill* item);
        void SetBillNumber(int number);
    protected:
    private:
        // Misc functions
        void CreateControls();
        void ConnectControls();
        void Update();
        void CheckClientDatas();
        // Events handlers
        void OnBtnAddClicked(wxCommandEvent &event);
        void OnBtnDelClicked(wxCommandEvent &event);
        void OnBtnEditClicked(wxCommandEvent &event);
        void OnBtnMoveUpClicked(wxCommandEvent &event);
        void OnBtnMoveDownClicked(wxCommandEvent &event);
        void OnBtnSelClientClicked(wxCommandEvent &event);
        void OnBtnLockClicked(wxCommandEvent &event);
        void OnScdDiscountChanged(wxSpinDoubleEvent &event);
        void OnChkPayNoteClicked(wxCommandEvent &event);
        void OnBtnValidClicked(wxCommandEvent &event);
        void OnUpdateUI_BtnValid(wxUpdateUIEvent &event);
        void OnUpdateUI_DpcTerm(wxUpdateUIEvent &event);
        void OnUpdateUI_BtnMoveUp(wxUpdateUIEvent &event);
        void OnUpdateUI_BtnMoveDown(wxUpdateUIEvent &event);
        void OnUpdateUI_VATCtrls(wxUpdateUIEvent &event);
        void OnUpdateUI_OthersCtrls(wxUpdateUIEvent &event);
        // Controls vars
        wxButton *m_btnAdd, *m_btnLock;
        wxDatePickerCtrl *m_dpcCreation, *m_dpcTerm;
        wxTimePickerCtrl *m_tpcCreation;
        wxCheckBox *m_chkTermDate;
        wxTextCtrl *m_txtNumber, *m_txtClient, *m_txtPayNote;
        wxButton *m_btnSelClient;
        wxScrolledWindow *m_scwItems;
        wxStaticText *m_lblTotal, *m_lblDiscount, *m_lblTotalDisc, *m_lblVAT, *m_lblTaxIncl;
        wxSpinCtrlDouble *m_scdDiscount, *m_scdVAT;
        wxCheckBox *m_chkShowVAT, *m_chkPayNote;
        wxPanel *m_pnlBottom;
        // Misc vars
        Bill* m_Bill;
        wxString m_sClientKey;
        bool m_bLocked;
};

#endif // __DLGADDEDITBILL_H_INCLUDED__
