#ifndef __PANELNBOOKPAGE_BILLS_H_INCLUDED__
#define __PANELNBOOKPAGE_BILLS_H_INCLUDED__

#include "panelnbookpage.h"

#include <wx/listctrl.h>

class PanelNBookPage_Bills : public PanelNBookPage
{
    public:
        PanelNBookPage_Bills(wxWindow* parent);
        virtual ~PanelNBookPage_Bills();
        void AddBillItem(Bill* item, bool select=false);
        virtual void RefreshDatasLists();
    protected:
    private:
        // Misc functions
        virtual void CreateControls();
        virtual void ConnectControls();
        long InsertBillItemToList(Bill* item, long before=wxID_ANY, bool select=false, bool updateWidths=false);
        void RefreshBillsList();
        void UpdateBillItem(long lItem);
        // Events handlers
        void OnOptDisplayClicked(wxCommandEvent &event);
        void OnBtnAddClicked(wxCommandEvent &event);
        void OnBtnDelClicked(wxCommandEvent &event);
        void OnBtnEditClicked(wxCommandEvent &event);
        void OnBtnSaveAsClicked(wxCommandEvent &event);
        void OnBtnCopyClicked(wxCommandEvent &event);
        void OnLstItemActivated(wxListEvent &event);
        void OnLstBillsColumnClicked(wxListEvent &event);
        void OnUpdateUI_ItemButtons(wxUpdateUIEvent &event);
        // Controls vars
        wxRadioButton *m_optDisplay[5];
        wxListView *m_lstBills;
        wxButton *m_btnAdd, *m_btnDel, *m_btnEdit, *m_btnSaveAs, *m_btnCopy;
        // Misc vars
        bool m_bColOrderAsc[3];
};

#endif // __PANELNBOOKPAGE_BILLS_H_INCLUDED__
