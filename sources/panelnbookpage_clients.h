#ifndef __PANELNBOOKPAGE_CLIENTS_H_INCLUDED__
#define __PANELNBOOKPAGE_CLIENTS_H_INCLUDED__

#include "panelnbookpage.h"

#include <wx/listctrl.h>
class Client;

enum
{
    CI_COL_COMPANY = 0,
    CI_COL_LASTNAME,
    CI_COL_FIRSTNAME,
    CI_COL_EMAIL,
    CI_COL_HOME_TEL,
    CI_COL_MOBILE,

    CI_COLS_COUNT
};

class PanelNBookPage_Clients : public PanelNBookPage
{
    public:
        PanelNBookPage_Clients(wxWindow* parent);
        virtual ~PanelNBookPage_Clients();
        virtual void RefreshDatasLists();
    protected:
    private:
        // Misc functions
        virtual void CreateControls();
        virtual void ConnectControls();
        void RefreshClientsList();
        long InsertClientItemToList(Client* item, long before=wxID_ANY, bool select=false, bool updateWidths=false);
        void UpdateClientItem(long lItem);
        void UpdateColumnsWidths();
        // Events handlers
        void OnTxtFilterChanged(wxCommandEvent &event);
        void OnCmbFilterFieldChanged(wxCommandEvent &event);
        void OnUpdateUI_Btn_Add_Del(wxUpdateUIEvent &event);
        void OnBtnAddClientClicked(wxCommandEvent &event);
        void OnBtnDelClientClicked(wxCommandEvent &event);
        void OnBtnEditClientClicked(wxCommandEvent &event);
        void OnLstClientItemActivated(wxListEvent &event);
        void OnLstClientsColumnClicked(wxListEvent &event);
        // Controls vars
        wxTextCtrl *m_txtFilter;
        wxChoice *m_cmbFilterField;
        wxStaticText *m_lblResults;
        wxListView *m_lstClients;
        wxButton *m_btnAddClient, *m_btnDelClient, *m_btnEditClient;
        // Misc vars
        static const wxString m_sColItem[CI_COLS_COUNT];
        int m_iMinWidth[CI_COLS_COUNT];
        bool m_bColOrderAsc[3];
};

#endif // __PANELNBOOKPAGE_CLIENTS_H_INCLUDED__
