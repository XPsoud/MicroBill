#ifndef __PANELNBOOKPAGE_ESTIMATES_H_INCLUDED__
#define __PANELNBOOKPAGE_ESTIMATES_H_INCLUDED__

#include "panelnbookpage.h"

#include <wx/listctrl.h>

enum
{
    EI_COL_DATE_CREATION = 0,
    EI_COL_CLIENT,
    EI_COL_VALUE,
    EI_COL_STATUS,
    EI_COL_DATE_LIMIT,

    EI_COLS_COUNT
};
extern const wxEventType wxEVT_CONVERT_ESTIM2BILL;

class PanelNBookPage_Estimates : public PanelNBookPage
{
    public:
        PanelNBookPage_Estimates(wxWindow* parent);
        virtual ~PanelNBookPage_Estimates();
        virtual void RefreshDatasLists();
    protected:
    private:
        // Misc functions
        virtual void CreateControls();
        virtual void ConnectControls();
        long InsertEstimateItemToList(Estimate* item, long before=wxID_ANY, bool select=false, bool updateWidths=false);
        void RefreshEstimatesList();
        void UpdateEstimateItem(long lItem);
        // Events handlers
        void OnOptDisplayClicked(wxCommandEvent &event);
        void OnBtnAddClicked(wxCommandEvent &event);
        void OnBtnDelClicked(wxCommandEvent &event);
        void OnBtnEditClicked(wxCommandEvent &event);
        void OnBtnSaveAsClicked(wxCommandEvent &event);
        void OnBtnCopyClicked(wxCommandEvent &event);
        void OnBtnBillClicked(wxCommandEvent &event);
        void OnLstItemActivated(wxListEvent &event);
        void OnLstEstimatesColumnClicked(wxListEvent &event);
        void OnUpdateUI_ItemButtons(wxUpdateUIEvent &event);
        // Controls vars
        wxRadioButton *m_optDisplay[5];
        wxListView *m_lstEstimates;
        wxButton *m_btnAdd, *m_btnDel, *m_btnEdit, *m_btnSaveAs, *m_btnCopy, *m_btnBill;
        // Misc vars
        int m_iMinWidth[EI_COLS_COUNT];
        bool m_bColOrderAsc[3];
};

#endif // __PANELNBOOKPAGE_ESTIMATES_H_INCLUDED__
