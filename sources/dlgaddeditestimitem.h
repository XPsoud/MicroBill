#ifndef __DLGADDEDITESTIMITEM_H_INCLUDED__
#define __DLGADDEDITESTIMITEM_H_INCLUDED__

#include <wx/wx.h>
#include <wx/spinctrl.h>

class DlgAddEditEstimItem : public wxDialog
{
    public:
        DlgAddEditEstimItem(wxWindow *parent);
        virtual ~DlgAddEditEstimItem();
        void SetItemDatas(const wxString& descr, const wxString& comments, double unitPrice, int quantity);
        wxString GetDescription() { return m_txtDescr->GetValue(); }
        wxString GetComments() { return m_txtComments->GetValue(); }
        double GetUnitPrice() { return m_spdUnitPrice->GetValue(); }
        int GetQuantity() { return m_spcQuantity->GetValue(); }
    protected:
    private:
        // Misc functions
        void CreateControls();
        void ConnectControls();
        // Events handlers
        void OnBtnValidClicked(wxCommandEvent &event);
        void OnUpdateUI_BtnValid(wxUpdateUIEvent &event);
        // Controls vars
        wxTextCtrl *m_txtDescr, *m_txtComments;
        wxSpinCtrlDouble *m_spdUnitPrice;
        wxSpinCtrl *m_spcQuantity;
        // Misc vars
};

#endif // __DLGADDEDITESTIMITEM_H_INCLUDED__
